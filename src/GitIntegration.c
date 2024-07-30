/*
 * GitIntegration.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: GIT operations for files.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <Windows.h>
#include <string.h>
#include <stringutil.h>
#include <Shlwapi.h>
#include "alloc.h"
#include "gitintegration.h"
#include "linkedlist.h"

#ifdef GITINTEGRATION
#include <git2.h>

struct tagVC_INFO {
	git_repository* vci_repository;
	VC_STATUS vci_status;
	git_oid   vci_id;
};

/*
 * Data structure, which is used to keep track of GIT repos to be able to quickly associate open files
 * with repos or find out, they are not tracked.
 */
typedef struct tagGIT_REPO_DIR {
	struct tagGIT_REPO_DIR* gr_next;
	char*			gr_path;
	int				gr_pathLen;
	git_repository* gr_repository;
	git_tree*		gr_root;
} GIT_REPO_DIR;

static BOOL gitInitialized;
static GIT_REPO_DIR* gitRepositories;

/*
 * Initialize the GIT library.
 */
static void gi_initialize() {
	if (gitInitialized) {
		return;
	}
	git_libgit2_init();
	gitInitialized = TRUE;
}

/*
 * Find a GIT repository for a given filename from our list of already opened GIT repos.
 */
static GIT_REPO_DIR* gi_findRepo(const char* pszFilename) {
	GIT_REPO_DIR* pDir = gitRepositories;
	while (pDir) {
		int len = PathCommonPrefix(pszFilename, pDir->gr_path, NULL);
		if (len == pDir->gr_pathLen) {
			return pDir;
		}
		pDir = pDir->gr_next;
	}
	return NULL;
}

/*
 * Convert a GIT path to a windows path.
 */
static void gi_deNormalizePath(char* pszPath) {
	while (*pszPath) {
		if (*pszPath == '/') {
			*pszPath = '\\';
		}
		pszPath++;
	}
}


/*
 * libgit assumes, that all pathes are delimited using the / character - normalize Windows path names to conform
 * with that assumption.
 */
static void gi_normalizePath(char* pszPath) {
	while (*pszPath) {
		if (*pszPath == '\\') {
			*pszPath = '/';
		}
		pszPath++;
	}
}

/*
 * Try to open a GIT repository.
 */
static GIT_REPO_DIR* gi_tryOpenRepo(const char* pszFilename) {
	char *pszDirectory = _strdup(pszFilename);
	if (pszDirectory == NULL) {
		return NULL;
	}
	string_splitFilename(pszFilename, pszDirectory, NULL, 0);
	GIT_REPO_DIR* pRepository = calloc(sizeof(GIT_REPO_DIR), 1);
	if (pRepository == NULL) {
		free(pszDirectory);
		return NULL;
	}
	git_repository_open_ext(&pRepository->gr_repository, pszDirectory, 0, NULL);
	if (pRepository->gr_repository) {
		git_object* obj = NULL;
		int error = git_revparse_single(&obj, pRepository->gr_repository, "HEAD^{tree}");
		if (!error) {
			pRepository->gr_root = (git_tree*)obj;
			const char* pszWorkdir = git_repository_workdir(pRepository->gr_repository);
			if (pszWorkdir != NULL) {
				free(pszDirectory);
				pszDirectory = _strdup(pszWorkdir);
				gi_deNormalizePath(pszDirectory);
			}
		} else {
			git_repository_free(pRepository->gr_repository);
			pRepository->gr_repository = NULL;
		}
	}
	pRepository->gr_path = pszDirectory;
	// subtract 1 - gr_path has trailing slash
	pRepository->gr_pathLen = (int)strlen(pszDirectory) - 1;
	ll_add(&gitRepositories, (LINKED_LIST*) pRepository);
	return pRepository;
}

static void gi_updateStatus(git_repository* pRepository, const char* szRelative, VC_INFO* vcInfo) {
	unsigned int status;
	int ret = git_status_file(&status, pRepository, szRelative);
	vcInfo->vci_status = VCS_NONE;
	if (ret == 0) {
		if (status == GIT_STATUS_CURRENT) {
			vcInfo->vci_status = VCS_CURRENT;
		}
		else if (status & GIT_STATUS_WT_MODIFIED) {
			vcInfo->vci_status = VCS_MODIFIED;
		}
		else if (status & GIT_STATUS_WT_DELETED) {
			vcInfo->vci_status = VCS_DELETED;
		}
		else if (status & GIT_STATUS_WT_NEW) {
			vcInfo->vci_status = VCS_NEW;
		}
		else if (status & GIT_STATUS_CONFLICTED) {
			vcInfo->vci_status = VCS_CONFLICTED;
		}
		else if (status & GIT_STATUS_IGNORED) {
			vcInfo->vci_status = VCS_IGNORED;
		}
	}
}

/*
 * Update the status of a file under version control (e.g. after the file has been changed).
 */
static void gi_updateVersionInfo(VC_INFO* pInfo, const char* pszFilename) {
	if (pInfo == NULL) {
		return;
	}
	GIT_REPO_DIR* pDir = gitRepositories;
	while (pDir) {
		if (pDir->gr_repository == pInfo->vci_repository) {
			char* pszRelative = _strdup(pszFilename + strlen(pDir->gr_path));
			if (pszRelative == NULL) {
				return;
			}
			gi_normalizePath(pszRelative);
			gi_updateStatus(pDir->gr_repository, pszRelative, pInfo);
			free(pszRelative);
			return;
		}
		pDir = pDir->gr_next;
	}
}

/*
 * Return the current version control information for a given file name.
 */
VC_INFO* gi_getVersionInfo(const char* pszFilename) {
	gi_initialize();
	GIT_REPO_DIR* pDir = gi_findRepo(pszFilename);
	if (pDir == NULL) {
		pDir = gi_tryOpenRepo(pszFilename);
	}
	unsigned int status = 0;
	if (!pDir->gr_repository) {
		return NULL;
	}
	char *pszRelative = _strdup(pszFilename + strlen(pDir->gr_path));
	gi_normalizePath(pszRelative);
	git_tree_entry* entry;
	VC_INFO* vcInfo = (VC_INFO*) calloc(sizeof *vcInfo, 1);
	if (vcInfo == NULL) {
		return NULL;
	}
	vcInfo->vci_repository = pDir->gr_repository;
	if (git_tree_entry_bypath(&entry, pDir->gr_root, pszRelative) == 0) {
		vcInfo->vci_id = *git_tree_entry_id(entry);
		git_tree_entry_free(entry);
	}
	gi_updateStatus(pDir->gr_repository, pszRelative, vcInfo);
	free(pszRelative);
	return vcInfo;
}

/*
 * Return the current GIT status for a given version info.
 */
VC_STATUS gi_getStatus(VC_INFO* pInfo, const char* pszFilename) {
	gi_updateVersionInfo(pInfo, pszFilename);
	return pInfo == NULL ? VCS_NONE : pInfo->vci_status;
}

/*
 * Print the hash of the current ref of the file loaded into pszBuffer with a maximum length of nLen+1. If nLen is 0
 * GIT_OID_SHA1_HEXSIZE  + 1 characters are used for printing, so pszBuffer must be at least that big.
 */
void gi_printHash(VC_INFO* pInfo, char* pszBuffer, int nLen) {
	if (pInfo == NULL) {
		pszBuffer[0] = 0;
		return;
	}
	if (nLen == 0) {
		nLen = GIT_OID_SHA1_HEXSIZE + 1;
	}
	git_oid_tostr(pszBuffer, nLen, &pInfo->vci_id);
	pszBuffer[nLen] = 0;
}

/*
 * Release the version information for a file.
 */
void gi_freeVersionInfo(VC_INFO* pVCInfo) {
	free(pVCInfo);
}

/*
 * Free a cached GIT repo including the working directory and root tree references.
 */
static gi_freeRepoDir(void* p) {
	GIT_REPO_DIR* pRepo = (GIT_REPO_DIR*)p;
	if (pRepo->gr_path) {
		free(pRepo->gr_path);
	}
	if (pRepo->gr_root) {
		git_tree_free(pRepo->gr_root);
	}
	if (pRepo->gr_repository) {
		git_repository_free(pRepo->gr_repository);
	}
	return 1;
}

/*
 * Shutdown the GIT integration.
 */
void gi_shutdown() {
	if (gitInitialized) {
		ll_destroy(&gitRepositories, gi_freeRepoDir);
		git_libgit2_shutdown();
		gitInitialized = FALSE;
	}
}

#else

/*
 * Print the hash of the current ref of the file loaded into pszBuffer with a maximum length of nLen+1. If nLen is 0
 * GIT_OID_SHA1_HEXSIZE  + 1 characters are used for printing, so pszBuffer must be at least that big.
 */
void gi_printHash(VC_INFO* pInfo, char* pszBuffer, int nLen) {
	pszBuffer[0] = 0;
}

/*
 * Shutdown the GIT integration.
 */
void gi_shutdown() {
}

VC_STATUS gi_getStatus(VC_INFO* pInfo, const char* pszFilename) {
	return VCS_NONE;
}

VC_INFO* gi_getVersionInfo(const char* pszFilename) {
	return NULL;
}

void gi_freeVersionInfo(VC_INFO* pVCInfo) {
}


#endif
