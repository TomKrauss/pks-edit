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
#include "alloc.h"
#include "gitintegration.h"
#include "linkedlist.h"
#include <git2.h>

#ifdef GIT_INTEGRATION

typedef struct tagGIT_REPO_DIR {
	struct tagGIT_REPO_DIR* gr_next;
	char* gr_path;
	git_repository* gr_repository;
} GIT_REPO_DIR;

static BOOL gitInitialized;
static GIT_REPO_DIR* gitRepositories;

static void gi_initialize() {
	if (gitInitialized) {
		return;
	}
	git_libgit2_init();
	gitInitialized = TRUE;
}

static GIT_REPO_DIR* gi_findRepo(const char* pszFilename) {
	GIT_REPO_DIR* pDir = gitRepositories;
	while (pDir) {
		if (strstr(pszFilename, pDir->gr_path) == pszFilename) {
			return pDir;
		}
		pDir = pDir->gr_next;
	}
	return NULL;
}

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
	string_splitFilename(pszFilename, pszDirectory, NULL);
	GIT_REPO_DIR* pRepository = calloc(sizeof(GIT_REPO_DIR), 1);
	git_repository_open_ext(&pRepository->gr_repository, pszDirectory, 0, NULL);
	if (pRepository->gr_repository) {
		const char* pszWorkdir = git_repository_workdir(pRepository->gr_repository);
		if (pszWorkdir != NULL) {
			free(pszDirectory);
		}
		pszDirectory = _strdup(pszWorkdir);
	}
	pRepository->gr_path = pszDirectory;
	ll_add(&gitRepositories, (LINKED_LIST*) pRepository);
	return pRepository;
}

/*
 * Return the current GIT status of a given file name.
 */
VC_STATUS gi_getStatus(const char* pszFilename) {
	gi_initialize();
	GIT_REPO_DIR* pDir = gi_findRepo(pszFilename);
	if (pDir == NULL) {
		pDir = gi_tryOpenRepo(pszFilename);
	} 
	unsigned int status = 0;
	if (!pDir->gr_repository) {
		return VCS_NONE;
	}
	char szRelative[MAX_PATH];
	strcpy(szRelative, pszFilename + strlen(pDir->gr_path));
	gi_normalizePath(szRelative);
	int ret = git_status_file(&status, pDir->gr_repository, szRelative);
	if (ret == 0) {
		if (status == GIT_STATUS_CURRENT) {
			return VCS_CURRENT;
		}
		if (status & GIT_STATUS_WT_MODIFIED) {
			return VCS_MODIFIED;
		}
		if (status & GIT_STATUS_WT_DELETED) {
			return VCS_DELETED;
		}
		if (status & GIT_STATUS_WT_NEW) {
			return VCS_NEW;
		}
		if (status & GIT_STATUS_CONFLICTED) {
			return VCS_CONFLICTED;
		}
	}
	return VCS_NONE;
}

static gi_freeRepoDir(void* p) {
	GIT_REPO_DIR* pRepo = (GIT_REPO_DIR*)p;
	if (pRepo->gr_path) {
		free(pRepo->gr_path);
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
 * Shutdown the GIT integration.
 */
void gi_shutdown() {
}

VC_STATUS gi_getStatus(const char* pszFilename) {
	return VCS_NONE;
}
#endif
