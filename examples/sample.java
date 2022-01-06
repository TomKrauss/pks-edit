//
// JdbcSession.java
//
// Sample copyright.....
//
//

package de.gebit.example.session.jdbc;

import java.sql.*;

/**
 * This class generically implements the session services within a JDBC context.
 * These basic services, like INSERT, SELECT and DELETE, are provided to interact
 * with a relational database.<P>
 * Moreover, this class is able to activate (this means, to connect to a
 * database by using the appropriate driver) and de-activate a session (to release
 * a database connection).</P>
 *
 * @author Oscar Wilde
 * @version 07 Apr 2010
 */
public class JdbcSession extends AbstractSession {
	/**
	 * Session property prefix to be used to define a table qualifier mapping.
	 */
	public static final 	 String 	  QUALIFIER_MAPPING_PROPERTY 		= "table\"QualifierMapping";

	/**
	 * Session property to be used to cause the session to consolidate updates and inserts of
	 * objects related by inheritance. This enables one to perform "morphing" operations on the
	 * database, by simply deleting an instance of A and inserting an instance of B with the
	 * same key, where A and B share a common persistent base class.
	 */
	public static final   String 	  CONSOLIDATE_DELETES_AND_INSERTS_PROPERTY 	= "consolidateUpdatesAndInserts";

	/**
	 * Session property to be used to define the SQL fragment class to be used.
	 * @see SqlFragment
	 */
	public static final String    SQL_FRAGMENT_CLASS_PROPERTY = "sqlFragment.class";

	// static variables
	private static final    String  QUALIFIER_SEPARATOR = ".";

	protected static final  String  PAD_CHARACTER_COLUMNS_WITH_BLANKS	 = "padCharacterColumnsWithBlanks";
	protected static final  String	DRIVER_CLASS_NAME_PROPERTY		= "driverClassName";
	protected static final  String  READ_AUTO_COMMIT_PROPERTY 		= "readAutoCommit";
	protected static final  String  READ_AUTO_COMMIT_PROPERTY_VALUE	= "TRUE";
	protected static final  String  READ_SERVICE_PACKAGE_PROPERTY	= "ReadServicePackage";
	protected static final  String  READ_SERVICE_SUBPACKAGE_PROPERTY	= "readservice";
	protected static final  String	DEADLOCK_TIMEOUT_MAXWAITTIME		= "deadlock_timeout_maxwaittime";
	protected static final  String	DEADLOCK_TIMEOUT_RETRIES			= "deadlock_timeout_retries";

	/**
	 * Prefix used in the session configuration to configure the mapping.
	 */
	public static final 	String  MAPPING_PROPERTY					= "mapping.";

	/**
	 * Table row locking is also used for resolved fields.
	 * For future use. It is currently not possible to use a Query object or Session hints for locking operations.
	 */
	public static final 	ParameterizedKey<Boolean> LOCK_RESOLVED_FIELDS = new ParameterizedKey<>("LockResolvedFields");    //$NON-NLS-1$
	/**
	 * Configuration property to configure optimistic locking.
	 */
	public static final     String	LOCKING_OPTIMISTIC_PROPERTY     = "locking.optimistic";
	/**
	 * Configuration property to configure optimistic locking compare strategy.
	 */
	public static final     String	LOCKING_COMPARE_PROPERTY        = "locking.compare";
	/**
	 * Configuration property to configure optimistic locking compare all strategy.
	 */
	public static final     String	LOCKING_METHOD_ALL				= "all";
	/**
	 * Configuration property to configure optimistic locking compare updated strategy.
	 */
	public static final     String 	LOCKING_METHOD_UPDATED			= "updated";
	/**
	 * Configuration property to configure optimistic locking compare enable on select strategy.
	 */
	public static final 	String	LOCKING_METHOD_ENABLE_SELECT	= "enableOnSelect";

	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String  SQLERROR_QUERY_CANCELLED = "57014";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_DUPKEY = "23505";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_COLUMN_TRUNCATED = "22001";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_DELETE_RESTRICT_INTEGRIY_VIOLATION = "23001";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_DELETE_NULL_INTEGRITY_VIOLATION = "23502";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_INSERT_UPDATE_INTEGRITY_VIOLATION = "23503";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_DELETE_NO_ACTION_INTEGRITY_VIOLATION = "23504";
	/**
	 * Common SQL error code, which should be common to all databases according to SQL standard
	 */
	public static final String	SQLERROR_DELETE_SOURCE_ROW_INTEGRITY_VIOLATION = "23511";

	/**
	 * Use this constant to register a session hint of "true" for TREND queries
	 * against a JDBC session to avoid a "DISTINCT" clause in the SQL generated.
	 */
	public static final ParameterizedKey<Boolean> QUERY_HINT_NO_DISTINCT = new ParameterizedKey<Boolean>("NO_DISTINCT"); //$NON-NLS-1$

	/**
	 * Use this constant to register an integer value in seconds to  be used as
	 * a timeout values for TREND queries.
	 */
	public static final ParameterizedKey<Integer> QUERY_HINT_TIMEOUT = new ParameterizedKey<>("QUERY_TIMEOUT"); //$NON-NLS-1$

	/**
	 * @see Query#QUERY_HINT_RESULTSET_OFFSET
	 */
	public static final ParameterizedKey<Number> QUERY_HINT_RESULTSET_OFFSET = Query.QUERY_HINT_RESULTSET_OFFSET; //$NON-NLS-1$

	/**
	 * Use this constant to specify an a limit below which the access layer avoids joins
	 * in dependent selects.
	 * The value must be an Integer {@code >= 0}.
	 */
	public static final ParameterizedKey<Integer> QUERY_HINT_MAX_SIZE_AVOID_JOINS = new ParameterizedKey<Integer>("AVOID_JOINS_UP_TO"); //$NON-NLS-1$

	/**
	 * The following RESOLVE_USE constants are boolean session hints, which are used
	 * to enable result class joins.
	 */

	/**
	 * Enables general usage of  single select joins for resolved fields.
	 */
	public static final ParameterizedKey<Boolean> RESOLVE_USE_JOINS = Query.RESOLVE_USE_JOINS;

	/**
	 * Enables outer joins. Setting is ignored if the session implementation does not support outer joins.
	 * Currently only the oracle session supports outer joins.
	 */
	public static final ParameterizedKey<Boolean> RESOLVE_USE_OUTER_JOINS = Query.RESOLVE_USE_OUTER_JOINS;

	/**
	 * Enables single select joins for to many relation.
	 * Attention: This setting increases the amount of data transfer from database to access layer. This may only be useful under special circumstances.
	 * Use this feature only if you know what you are doing.
	 */
	public static final ParameterizedKey<Boolean> RESOLVE_USE_JOIN_FOR_TO_MANY = new ParameterizedKey<>("resolvedFields.UseJoinsForToMany"); //$NON-NLS-1$

	/**
	 * Advanced feature to limit the amount of data that is read from the database. If this property is set only these attributes are read which
	 * are specified in the property value. This is mainly useful for browser views, where only a small portion is shown as an overview.
	 * It is strongly recommended to completely specify the persistent key, since the upper layers of the access layer need them to process to BOs.
	 */
	public static final ParameterizedKey<String[]> READ_PARTIAL_BOS_ATTRIBUTE_LIST = new ParameterizedKey<>("readPartialBOs.AttributeList"); //$NON-NLS-1$

	/**
	 * Very advanced feature which only makes sense if you execute a TrendQuery from a CustomOperation or CustomReadService and uses the @see #READ_PARTIAL_BOS_ATTRIBUTE_LIST
	 * property to get a compacted view on some non-key attributes.
	 */
	public static final ParameterizedKey<Boolean> ENFORCE_SELECT_DISTINCT = new ParameterizedKey<>("USE_DISTINCT"); //$NON-NLS-1$

	/**
	 * Constant to identify JDBC level 2.0
	 */
	public static final Version JDBC_2_0 = new Version(2, 0);
	/**
	 * Constant to identify JDBC level 3.0
	 */
	public static final Version JDBC_3_0 = new Version(3, 0);
	/**
	 * Constant to identify JDBC level 4.0
	 */
	public static final Version JDBC_4_0 = new Version(4, 0);
	/**
	 * Constant to identify JDBC level 4.2
	 */
	public static final Version JDBC_4_2 = new Version(4, 2);


	// instance variables
	protected SqlFragment 						sqlFragment;
	protected Integer								isolationLevel;
	protected boolean           					optimisticLockingEnabled = true;
	protected boolean           					optimisticLockingIgnoreFloat;
	protected boolean								ignoreNoRowsAffected;
	protected Set<String>							tablesExcludedForOptimisticLocking;
	protected OptimisticLocking						optimisticLockingMode = OptimisticLocking.COMPARE_ALL;
	protected Boolean           					readAutoCommit;
	protected Boolean								padCharacterColumnsWithBlanks;
	protected Connection        					connection;
	protected DataSource							dataSource;
	private Collection<DeleteRequest>         deleteRequests;
	private Collection<InsertRequest> 			insertRequests;
	private Collection<UpdateRequest>        updateRequests;
	private boolean									consolidateInsertUpdates;
	protected JdbcSessionConfigurationObject 		sessionParameters;
	protected Set<String>    						checkColumnNames;
	protected String									insertUpdateDeleteOrder;
	protected int										slowSqlThreshold = 100;
	private int										binaryAsStreamLimit = 0;
	private int										maxStatementsPerBatch;
	private boolean									optimizeBatchInserts;
	private boolean									supportsBatchUpdates;
	private boolean									logRetryableSessionExceptions = false;
	private boolean									useUTCForTimestamps = true;
	private SessionDispenser					sessionDispenser;
	private Version jdbcVersion;
	private Random random;

	/** Complete DB URL that was used to get the connection from the DriverManager. */
//	protected String        	connectionURL;
//	protected String			datasourceName;

	/**
	 * If not specified in session hints of query then use selected records for finding
	 * depending objects.
	 */
	protected int defaultAvoidJoinsSize = 50;

	/**
	 * Called by the session dispenser after creation.
	 */
	public void setSessionDispenser(SessionDispenser aSessionDispenser) {
		sessionDispenser = aSessionDispenser;
	}

	/**
	 * Aborts a transaction (M).
	 *
	 */
	@Override
	public void abortTransaction() throws SessionException {
		getLogger().debug("abortTransaction");
		resetRequestCollections();
		if (shouldRollbackOnAbortTransaction()) {
			rollbackTransaction();
		}
	}

	/**
	 * Returns true if the DB transaction should be rolled back on abortTransaction.
	 */
	protected boolean shouldRollbackOnAbortTransaction()  {
		try {
			return !isReadAutoCommitEnabled() &&
				Boolean.TRUE.equals(getSessionParameters().getHoldSessionAfterBeginTransaction());
		} catch (Exception e) {
			getLogger().error("Exception occured", e);
			return false;
		}
	}
}
