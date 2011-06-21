/*
* libvirt-php.c: Core of the PHP bindings library/module
*
* See COPYING for the license of this software
*
* Written by:
*   Radek Hladik <r.hladik@cybersales.cz>
*   Michal Novotny <minovotn@redhat.com>
*   David King
*   Jan-Paul van Burgsteden
*   Lyre <liyong@skybility.com> (or <4179e1@gmail.com>)
*   Daniel P. Berrange <berrange@redhat.com>
*   Tiziano Mueller <dev-zero@gentoo.org>
*   Yukihiro Kawada <warp.kawada@gmail.com>
*/

#include "libvirt-php.h"

#ifdef DEBUG_CORE
#define DPRINTF(fmt, ...) \
if (LIBVIRT_G(debug)) \
do { fprintf(stderr, "[%s ", get_datetime()); fprintf(stderr, "libvirt-php/core]: " fmt , ## __VA_ARGS__); fflush(stderr); } while (0)
#else
#define DPRINTF(fmt, ...) \
do {} while(0)
#endif

/* PHP functions are prefixed with `zif_` so strip it */
#define	PHPFUNC	(__FUNCTION__ + 4)

/* Additional binaries */
char *features[] = { "screenshot", NULL };
char *features_binaries[] = { "/usr/bin/gvnccapture", NULL };

/* ZEND thread safe per request globals definition */
int le_libvirt_connection;
int le_libvirt_domain;
int le_libvirt_storagepool;
int le_libvirt_volume;
int le_libvirt_network;
int le_libvirt_nodedev;
int le_libvirt_snapshot;

ZEND_DECLARE_MODULE_GLOBALS(libvirt)

ZEND_BEGIN_ARG_INFO_EX(arginfo_libvirt_connect, 0, 0, 0)
	ZEND_ARG_INFO(0, url)
	ZEND_ARG_INFO(0, readonly)
ZEND_END_ARG_INFO()

static function_entry libvirt_functions[] = {
	/* Common functions */
	PHP_FE(libvirt_get_last_error,NULL)
	/* Connect functions */
	PHP_FE(libvirt_connect, arginfo_libvirt_connect)
	PHP_FE(libvirt_connect_get_uri, NULL)
	PHP_FE(libvirt_connect_get_hostname, NULL)
	PHP_FE(libvirt_connect_get_capabilities, NULL)
	PHP_FE(libvirt_connect_get_information, NULL)
	PHP_FE(libvirt_connect_get_hypervisor, NULL)
	PHP_FE(libvirt_connect_get_sysinfo, NULL)
	PHP_FE(libvirt_connect_get_maxvcpus, NULL)
	PHP_FE(libvirt_connect_get_encrypted, NULL)
	PHP_FE(libvirt_connect_get_secure, NULL)
	/* Domain functions */
	PHP_FE(libvirt_domain_get_counts, NULL)
	PHP_FE(libvirt_domain_lookup_by_name, NULL)
	PHP_FE(libvirt_domain_get_xml_desc, NULL)
	PHP_FE(libvirt_domain_change_vcpus, NULL)
	PHP_FE(libvirt_domain_change_memory, NULL)
	PHP_FE(libvirt_domain_change_boot_devices, NULL)
	PHP_FE(libvirt_domain_disk_add, NULL)
	PHP_FE(libvirt_domain_disk_remove, NULL)
	PHP_FE(libvirt_domain_nic_add, NULL)
	PHP_FE(libvirt_domain_nic_remove, NULL)
	PHP_FE(libvirt_domain_get_info, NULL)
	PHP_FE(libvirt_domain_get_name, NULL)
	PHP_FE(libvirt_domain_get_uuid, NULL)
	PHP_FE(libvirt_domain_get_uuid_string, NULL)
	PHP_FE(libvirt_domain_get_id, NULL)
	PHP_FE(libvirt_domain_lookup_by_id, NULL)
	PHP_FE(libvirt_domain_lookup_by_uuid, NULL)
	PHP_FE(libvirt_domain_lookup_by_uuid_string, NULL)
	PHP_FE(libvirt_domain_destroy, NULL)
	PHP_FE(libvirt_domain_create, NULL)
	PHP_FE(libvirt_domain_resume, NULL)
	PHP_FE(libvirt_domain_core_dump, NULL)
	PHP_FE(libvirt_domain_shutdown, NULL)
	PHP_FE(libvirt_domain_suspend, NULL)
	PHP_FE(libvirt_domain_undefine, NULL)
	PHP_FE(libvirt_domain_reboot, NULL)
	PHP_FE(libvirt_domain_define_xml, NULL)
	PHP_FE(libvirt_domain_create_xml, NULL)
	PHP_FE(libvirt_domain_memory_peek,NULL)
	PHP_FE(libvirt_domain_memory_stats,NULL)
	PHP_FE(libvirt_domain_block_stats,NULL)
	PHP_FE(libvirt_domain_interface_stats,NULL)
	PHP_FE(libvirt_domain_get_connect, NULL)
	PHP_FE(libvirt_domain_migrate, NULL)
	PHP_FE(libvirt_domain_migrate_to_uri, NULL)
	PHP_FE(libvirt_domain_get_job_info, NULL)
	PHP_FE(libvirt_domain_xml_xpath, NULL)
	PHP_FE(libvirt_domain_get_block_info, NULL)
	PHP_FE(libvirt_domain_get_network_info, NULL)
	PHP_FE(libvirt_domain_get_autostart, NULL)
	PHP_FE(libvirt_domain_set_autostart, NULL)
	PHP_FE(libvirt_domain_is_active, NULL)
	PHP_FE(libvirt_domain_get_next_dev_ids, NULL)
	PHP_FE(libvirt_domain_get_screenshot, NULL)
	PHP_FE(libvirt_domain_send_keys, NULL)
	PHP_FE(libvirt_domain_send_pointer_event, NULL)
	/* Domain snapshot functions */
	PHP_FE(libvirt_domain_has_current_snapshot, NULL)
	PHP_FE(libvirt_domain_snapshot_create, NULL)
	PHP_FE(libvirt_domain_snapshot_get_xml, NULL)
	PHP_FE(libvirt_domain_snapshot_revert, NULL)
	PHP_FE(libvirt_domain_snapshot_delete, NULL)
	PHP_FE(libvirt_domain_snapshot_lookup_by_name, NULL)
	/* Storagepool functions */
	PHP_FE(libvirt_storagepool_lookup_by_name,NULL)
	PHP_FE(libvirt_storagepool_lookup_by_volume,NULL)
	PHP_FE(libvirt_storagepool_get_info,NULL)
	PHP_FE(libvirt_storagevolume_lookup_by_name,NULL)
	PHP_FE(libvirt_storagevolume_lookup_by_path,NULL)
	PHP_FE(libvirt_storagevolume_get_name,NULL)
	PHP_FE(libvirt_storagevolume_get_path,NULL)
	PHP_FE(libvirt_storagevolume_get_info,NULL)
	PHP_FE(libvirt_storagevolume_get_xml_desc,NULL)
	PHP_FE(libvirt_storagevolume_create_xml,NULL)
	PHP_FE(libvirt_storagevolume_create_xml_from,NULL)
	PHP_FE(libvirt_storagevolume_delete,NULL)
	PHP_FE(libvirt_storagepool_get_uuid_string, NULL)
	PHP_FE(libvirt_storagepool_get_name, NULL)
	PHP_FE(libvirt_storagepool_lookup_by_uuid_string, NULL)
	PHP_FE(libvirt_storagepool_get_xml_desc, NULL)
	PHP_FE(libvirt_storagepool_define_xml, NULL)
	PHP_FE(libvirt_storagepool_undefine, NULL)
	PHP_FE(libvirt_storagepool_create, NULL)
	PHP_FE(libvirt_storagepool_destroy, NULL)
	PHP_FE(libvirt_storagepool_is_active, NULL)
	PHP_FE(libvirt_storagepool_get_volume_count, NULL)
	PHP_FE(libvirt_storagepool_refresh, NULL)
	PHP_FE(libvirt_storagepool_set_autostart, NULL)
	PHP_FE(libvirt_storagepool_get_autostart, NULL)
	PHP_FE(libvirt_storagepool_build, NULL)
	/* Network functions */
	PHP_FE(libvirt_network_define_xml, NULL)
	PHP_FE(libvirt_network_undefine, NULL)
	PHP_FE(libvirt_network_get, NULL)
	PHP_FE(libvirt_network_get_xml_desc, NULL)
	PHP_FE(libvirt_network_get_bridge, NULL)
	PHP_FE(libvirt_network_get_information, NULL)
	PHP_FE(libvirt_network_get_active, NULL)
	PHP_FE(libvirt_network_set_active, NULL)
	/* Node functions */
	PHP_FE(libvirt_node_get_info,NULL)
	/* Nodedev functions */
	PHP_FE(libvirt_nodedev_get, NULL)
	PHP_FE(libvirt_nodedev_capabilities, NULL)
	PHP_FE(libvirt_nodedev_get_xml_desc, NULL)
	PHP_FE(libvirt_nodedev_get_information, NULL)
	/* List functions */
	PHP_FE(libvirt_list_domains, NULL)
	PHP_FE(libvirt_list_domain_snapshots, NULL)
	PHP_FE(libvirt_list_domain_resources, NULL)
	PHP_FE(libvirt_list_nodedevs, NULL)
	PHP_FE(libvirt_list_networks,NULL)
	PHP_FE(libvirt_list_storagepools,NULL)
	PHP_FE(libvirt_list_active_storagepools,NULL)
	PHP_FE(libvirt_list_inactive_storagepools,NULL)
	PHP_FE(libvirt_storagepool_list_volumes,NULL)
	PHP_FE(libvirt_list_active_domains, NULL)
	PHP_FE(libvirt_list_active_domain_ids, NULL)
	PHP_FE(libvirt_list_inactive_domains, NULL)
	/* Version information function */
	PHP_FE(libvirt_version, NULL)
	PHP_FE(libvirt_check_version, NULL)
	PHP_FE(libvirt_has_feature, NULL)
	PHP_FE(libvirt_get_iso_images, NULL)
	/* Debugging functions */
	PHP_FE(libvirt_logfile_set, NULL)
	PHP_FE(libvirt_print_binding_resources, NULL)
	{NULL, NULL, NULL}
};


/* Zend module basic definition  */
zend_module_entry libvirt_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_LIBVIRT_WORLD_EXTNAME,
    libvirt_functions,
    PHP_MINIT(libvirt),
    PHP_MSHUTDOWN(libvirt),
    PHP_RINIT(libvirt),
    PHP_RSHUTDOWN(libvirt),
    PHP_MINFO(libvirt),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_LIBVIRT_WORLD_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_LIBVIRT
ZEND_GET_MODULE(libvirt)
#endif

/* PHP init options */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("libvirt.longlong_to_string", "1", PHP_INI_ALL, OnUpdateBool, longlong_to_string_ini, zend_libvirt_globals, libvirt_globals)
	STD_PHP_INI_ENTRY("libvirt.iso_path", "/var/lib/libvirt/images/iso", PHP_INI_ALL, OnUpdateString, iso_path_ini, zend_libvirt_globals, libvirt_globals)
PHP_INI_END()

void change_debug(int val)
{
	LIBVIRT_G(debug) = val;
	gdebug = val;
}

/* PHP requires to have this function defined */
static void php_libvirt_init_globals(zend_libvirt_globals *libvirt_globals)
{
	libvirt_globals->longlong_to_string_ini = 1;
	libvirt_globals->iso_path_ini = "/var/lib/libvirt/images/iso";
	libvirt_globals->binding_resources_count = 0;
	libvirt_globals->binding_resources = NULL;
	#ifdef DEBUG_SUPPORT
	libvirt_globals->debug = 0;
	change_debug(0);
	#endif
}

/* PHP request initialization */
PHP_RINIT_FUNCTION(libvirt)
{
	LIBVIRT_G(last_error) = NULL;
	change_debug(0);
	return SUCCESS;
}

/* PHP request destruction */
PHP_RSHUTDOWN_FUNCTION(libvirt)
{
	if (LIBVIRT_G (last_error)!=NULL) efree(LIBVIRT_G (last_error));
	return SUCCESS;
}

/*
	Private function name:	get_datetime
	Since version:		0.4.2
	Description:		Function can be used to get date and time in the `YYYY-mm-dd HH:mm:ss` format, internally used for logging when debug logging is enabled using libvirt_set_logfile() API function.
	Arguments:		None
	Returns:		Date/time string in `YYYY-mm-dd HH:mm:ss` format
*/
char *get_datetime(void)
{
	/* Caution: Function cannot use DPRINTF() macro otherwise the neverending loop will be met! */
	char *outstr = NULL;
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL)
		return NULL;

	outstr = (char *)malloc( 32 * sizeof(char) );
	if (strftime(outstr, 32, "%Y-%m-%d %H:%M:%S", tmp) == 0)
		return NULL;

	return outstr;
}

/*
	Private function name:	set_logfile
	Since version:		0.4.2
	Description:		Function to set the log file. You can set log file to NULL to disable logging (default). Useful for debugging purposes.
	Arguments:		@filename [string]: name of log file or NULL to disable
				@maxsize [long]: integer value of maximum file size, file will be truncated after reaching max file size. Value is set in KiB.
	Returns:		0 on success, -errno otherwise
*/
int set_logfile(char *filename, long maxsize)
{
	int res;
	struct stat st;

	if (filename == NULL) {
		change_debug(0);
		return 0;
	}

	/* Convert from KiB to bytes and check whether file size exceeds maxsize */
	maxsize *= 1024;
	if (access(filename, F_OK) == 0) {
		stat(filename, &st);
		if (st.st_size > maxsize)
			unlink(filename);
	}

	res = (freopen(filename, "a", stderr) != NULL) ? 0 : -errno;
	if (res == 0)
		change_debug(1);
	return res;
}

/*
	Private function name:	translate_counter_type
	Since version:		0.4.2
	Description:		Function to translate the counter type into the string format
	Arguments:		@type [int]: integer identifier of the counter type
	Returns:		string interpretation of the counter type
*/
char *translate_counter_type(int type)
{
	switch (type) {
		case INT_RESOURCE_CONNECTION:	return "connection";
						break;
		case INT_RESOURCE_DOMAIN:	return "domain";
						break;
		case INT_RESOURCE_NETWORK:	return "network";
						break;
		case INT_RESOURCE_NODEDEV:	return "node device";
						break;
		case INT_RESOURCE_STORAGEPOOL:	return "storage pool";
						break;
		case INT_RESOURCE_VOLUME:	return "storage volume";
						break;
		case INT_RESOURCE_SNAPSHOT:	return "snapshot";
						break;
	}

	return "unknown";
}

/*
	Private function name:	resource_change_counter
	Since version:		0.4.2
	Description:		Function to increment (inc = 1) / decrement (inc = 0) the resource pointers count including the memory location
	Arguments:		@type [int]: type of resource (INT_RESOURCE_x defines where x can be { CONNECTION | DOMAIN | NETWORK | NODEDEV | STORAGEPOOL | VOLUME | SNAPSHOT })
				@conn [virConnectPtr]: libvirt connection pointer associated with the resource, NULL for libvirt connection objects
				@mem [pointer]: Pointer to memory location for the resource. Will be converted to appropriate uint for the arch.
				@inc [int/bool]: Increment the counter (1 = add memory location) or decrement the counter (0 = remove memory location) from entries.
	Returns:		0 on success, -errno otherwise
*/
int resource_change_counter(int type, virConnectPtr conn, void *memp, int inc)
{
	int i;
	int pos = -1;
	int binding_resources_count;
	char tmp[64] = { 0 };
	arch_uint mem = 0;
	resource_info *binding_resources = NULL;

	snprintf(tmp, sizeof(tmp), "%p", memp);
	sscanf(tmp, "%x", &mem);

	binding_resources_count = LIBVIRT_G(binding_resources_count);
	binding_resources = LIBVIRT_G(binding_resources);

	if (inc) {
		for (i = 0; i < binding_resources_count; i++) {
			if (binding_resources[i].overwrite) {
				pos = i;
				break;
			}
			if ((binding_resources[i].type == type) || (binding_resources[i].mem == mem))
				return -EEXIST;
		}

		if (pos == -1) {
			if (binding_resources == NULL) {
				binding_resources_count = 1;
				binding_resources = (resource_info *)malloc( sizeof(resource_info) );
			}
			else {
				binding_resources_count++;
				binding_resources = (resource_info *)realloc( binding_resources, binding_resources_count * sizeof(resource_info) );
			}

			if (binding_resources == NULL)
				return -ENOMEM;

			pos = binding_resources_count - 1;
		}

		binding_resources[pos].type = type;
		binding_resources[pos].mem  = mem;
		binding_resources[pos].conn = conn;
		binding_resources[pos].overwrite = 0;
	}
	else {
		for (i = 0; i < binding_resources_count; i++) {
			if ((binding_resources[i].type == type) && (binding_resources[i].mem == mem))
				binding_resources[i].overwrite = 1;
		}
	}

	LIBVIRT_G(binding_resources_count) = binding_resources_count;
	LIBVIRT_G(binding_resources) = binding_resources;

	return 0;
}

/*
	Private function name:	get_feature_binary
	Since version:		0.4.1(-3)
	Description:		Function to get the existing feature binary for the specified feature, e.g. screenshot feature
	Arguments:		@name [string]: name of the feature to check against
	Returns:		Existing and executable binary name or NULL value
*/
char *get_feature_binary(char *name)
{
	int i, max;

	max = (ARRAY_CARDINALITY(features) < ARRAY_CARDINALITY(features_binaries)) ?
		ARRAY_CARDINALITY(features) : ARRAY_CARDINALITY(features_binaries);

	for (i = 0; i < max; i++)
		if ((features[i] != NULL) && (strcmp(features[i], name) == 0)) {
			if (access(features_binaries[i], X_OK) == 0)
				return strdup(features_binaries[i]);
		}

	return NULL;
}

/* Information function for phpinfo() */
PHP_MINFO_FUNCTION(libvirt)
{
	int i;
	char path[1024];
	char tmp[1024] = { 0 };
	unsigned long libVer;
	php_info_print_table_start();
	php_info_print_table_row(2, "Libvirt support", "enabled");

	#ifdef DEBUG_SUPPORT
		snprintf(tmp, sizeof(tmp), "enabled, default maximum log file size: %d KiB", DEFAULT_LOG_MAXSIZE);
	#else
		snprintf(tmp, sizeof(tmp), "disabled");
	#endif

	php_info_print_table_row(2, "Debug support", tmp);
	php_info_print_table_row(2, "Extension version", PHP_LIBVIRT_WORLD_VERSION);

	if (virGetVersion(&libVer,NULL,NULL)== 0)
	{
		char version[100];
		snprintf(version, sizeof(version), "%i.%i.%i", (long)((libVer/1000000) % 1000),(long)((libVer/1000) % 1000),(long)(libVer % 1000));
		php_info_print_table_row(2, "Libvirt version", version);
	}

	php_info_print_table_row(2, "Convertion of long long values to strings",
				(LIBVIRT_G(longlong_to_string_ini)) ? "True" : "False");
	if (!access(LIBVIRT_G(iso_path_ini), F_OK) == 0)
		snprintf(path, sizeof(path), "%s - path is invalid. To set the valid path modify the libvirt.iso_path in your php.ini configuration!",
					LIBVIRT_G(iso_path_ini));
	else
		snprintf(path, sizeof(path), "%s", LIBVIRT_G(iso_path_ini));

	php_info_print_table_row(2, "ISO Image path", path);

	/* Iterate all the features supported */
	char features_supported[4096] = { 0 };
	for (i = 0; i < ARRAY_CARDINALITY(features); i++) {
		char *tmp;
		if ((features[i] != NULL) && (tmp = get_feature_binary(features[i]))) {
			free(tmp);
			strcat(features_supported, features[i]);
			strcat(features_supported, ", ");
		}
	}

	if (strlen(features_supported) > 0) {
		features_supported[ strlen(features_supported) - 2 ] = 0;
		php_info_print_table_row(2, "Features supported", features_supported);
	}

	php_info_print_table_end();
}

/*
	Private function name:	set_error
	Since version:		0.4.1(-1)
	Description:		This private function is used to set the error string to the library. This string can be obtained by libvirt_get_last_error() from the PHP application.
	Arguments:		@msg [string]: error message string
	Returns:		None
*/
void set_error(char *msg TSRMLS_DC)
{
	if (LIBVIRT_G (last_error) != NULL)
		efree(LIBVIRT_G (last_error));

	if (msg == NULL)
		LIBVIRT_G (last_error) = NULL;
		return;

	php_error_docref(NULL TSRMLS_CC, E_WARNING,"%s",msg);
	LIBVIRT_G (last_error)=estrndup(msg,strlen(msg));
}

/*
	Private function name:	set_error_if_unset
	Since version:		0.4.2
	Description:		Function to set the error only if no other error is set yet
	Arguments:		@msg [string]: error message string
	Returns:		None
*/
void set_error_if_unset(char *msg TSRMLS_DC)
{
	if (LIBVIRT_G (last_error) == NULL)
		set_error(msg);
}

/*
	Private function name:	reset_error
	Since version:		0.4.2
	Description:		Function to reset the error string set by set_error(). Same as set_error(NULL).
	Arguments:		None
	Returns:		None
*/
void reset_error()
{
	set_error(NULL);
}


/* Error handler for receiving libvirt errors */
static void catch_error(void *userData,
                           virErrorPtr error)
{
	TSRMLS_FETCH_FROM_CTX(userData);
	set_error(error->message TSRMLS_CC);
}

/*
	Private function name:	free_resource
	Since version:		0.4.2
	Description:		Function is used to free the the internal libvirt-php resource identified by it's type and memory location
	Arguments:		@type [int]: type of the resource to be freed, INT_RESOURCE_x where x can be { CONNECTION | DOMAIN | NETWORK | NODEDEV | STORAGEPOOL | VOLUME | SNAPSHOT }
				@mem [uint]: memory location of the resource to be freed
	Returns:		None
*/
void free_resource(int type, arch_uint mem)
{
	int rv;

	DPRINTF("Freeing libvirt %s resource at 0x%llx\n", translate_counter_type(type), mem);

	if (type == INT_RESOURCE_DOMAIN) {
		rv = virDomainFree( (virDomainPtr)mem );
		if (rv != 0) {
			DPRINTF("%s: virDomainFree(%p) returned %d (%s)\n", __FUNCTION__, (virDomainPtr)mem, rv, LIBVIRT_G (last_error));
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virDomainFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
		}
		else {
			DPRINTF("%s: virDomainFree(%p) completed successfully\n", __FUNCTION__, (virDomainPtr)mem);
			resource_change_counter(INT_RESOURCE_DOMAIN, NULL, (virDomainPtr)mem, 0);
		}
	}

	if (type == INT_RESOURCE_NETWORK) {
		rv = virNetworkFree( (virNetworkPtr)mem );
		if (rv != 0) {
			DPRINTF("%s: virNetworkFree(%p) returned %d (%s)\n", __FUNCTION__, (virNetworkPtr)mem, rv, LIBVIRT_G (last_error));
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virNetworkFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
		}
		else {
			DPRINTF("%s: virNetworkFree(%p) completed successfully\n", __FUNCTION__, (virNetworkPtr)mem);
			resource_change_counter(INT_RESOURCE_NETWORK, NULL, (virNetworkPtr)mem, 0);
		}
	}

	if (type == INT_RESOURCE_NODEDEV) {
		rv = virNodeDeviceFree( (virNodeDevicePtr)mem );
		if (rv != 0) {
			DPRINTF("%s: virNodeDeviceFree(%p) returned %d (%s)\n", __FUNCTION__, (virNodeDevicePtr)mem, rv, LIBVIRT_G (last_error));
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virNodeDeviceFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
		}
		else {
			DPRINTF("%s: virNodeDeviceFree(%p) completed successfully\n", __FUNCTION__, (virNodeDevicePtr)mem);
			resource_change_counter(INT_RESOURCE_NODEDEV, NULL, (virNodeDevicePtr)mem, 0);
		}
	}

	if (type == INT_RESOURCE_STORAGEPOOL) {
		rv = virStoragePoolFree( (virStoragePoolPtr)mem );
		if (rv != 0) {
			DPRINTF("%s: virStoragePoolFree(%p) returned %d (%s)\n", __FUNCTION__, (virStoragePoolPtr)mem, rv, LIBVIRT_G (last_error));
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStoragePoolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
		}
		else {
			DPRINTF("%s: virStoragePoolFree(%p) completed successfully\n", __FUNCTION__, (virStoragePoolPtr)mem);
			resource_change_counter(INT_RESOURCE_STORAGEPOOL, NULL, (virStoragePoolPtr)mem, 0);
		}
	}

	if (type == INT_RESOURCE_VOLUME) {
		rv = virStorageVolFree( (virStorageVolPtr)mem );
		if (rv != 0) {
			DPRINTF("%s: virStorageVolFree(%p) returned %d (%s)\n", __FUNCTION__, (virStorageVolPtr)mem, rv, LIBVIRT_G (last_error));
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStorageVolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
		}
		else {
			DPRINTF("%s: virStorageVolFree(%p) completed successfully\n", __FUNCTION__, (virStorageVolPtr)mem);
			resource_change_counter(INT_RESOURCE_VOLUME, NULL, (virStorageVolPtr)mem, 0);
		}
	}

	if (type == INT_RESOURCE_SNAPSHOT) {
		rv = virDomainSnapshotFree( (virDomainSnapshotPtr)mem );
		if (rv != 0) {
			DPRINTF("%s: virDomainSnapshotFree(%p) returned %d (%s)\n", __FUNCTION__, (virDomainSnapshotPtr)mem, rv, LIBVIRT_G (last_error));
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virDomainSnapshotFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
		}
		else {
			DPRINTF("%s: virDomainSnapshotFree(%p) completed successfully\n", __FUNCTION__, (virDomainSnapshotPtr)mem);
			resource_change_counter(INT_RESOURCE_SNAPSHOT, NULL, (virDomainSnapshotPtr)mem, 0);
		}
	}
}

/*
	Private function name:	free_resources_on_connection
	Since version:		0.4.2
	Description:		Function is used to free all the resources assigned to the connection identified by conn
	Arguments:		@conn [virConnectPtr]: libvirt connection pointer
	Returns:		None
*/
void free_resources_on_connection(virConnectPtr conn)
{
	int binding_resources_count = 0;
	resource_info *binding_resources;
	int i;

	binding_resources_count = LIBVIRT_G(binding_resources_count);
	binding_resources = LIBVIRT_G(binding_resources);

	for (i = 0; i < binding_resources_count; i++) {
		if ((binding_resources[i].conn == conn) && (binding_resources[i].overwrite == 0))
			free_resource(binding_resources[i].type, binding_resources[i].mem);
	}
}

/*
	Private function name:	check_resource_allocation
	Since version:		0.4.2
	Description:		Function is used to check whether the resource identified by type and memory is allocated for connection conn or not
	Arguments:		@conn [virConnectPtr]: libvirt connection pointer
				@type [int]: type of the counter to be checked, please see free_resource() API for possible values
				@memp [pointer]: pointer to the memory
	Returns:		1 if resource is allocated, 0 otherwise
*/
int check_resource_allocation(virConnectPtr conn, int type, void *memp)
{
	int binding_resources_count = 0;
	resource_info *binding_resources = NULL;
	int i, allocated = 0;
	char tmp[64] = { 0 };
	arch_uint mem = 0;

	snprintf(tmp, sizeof(tmp), "%p", memp);
	sscanf(tmp, "%x", &mem);

	binding_resources_count = LIBVIRT_G(binding_resources_count);
	binding_resources = LIBVIRT_G(binding_resources);

	if (binding_resources == NULL)
		return 0;

	for (i = 0; i < binding_resources_count; i++) {
		if ((((conn != NULL) && (binding_resources[i].conn == conn)) || (conn == NULL))
			&& (binding_resources[i].type == type) && (binding_resources[i].mem == mem)
			&& (binding_resources[i].overwrite == 0))
				allocated = 1;
	}

	DPRINTF("%s: libvirt %s resource (conn %p) is%s allocated\n", __FUNCTION__, translate_counter_type(type),
		memp, !allocated ? " not" : "");
	return allocated;
}

/* Destructor for connection resource */
static void php_libvirt_connection_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_connection *conn = (php_libvirt_connection*)rsrc->ptr;
	int rv = 0;
	
	if (conn != NULL)
	{
		if (conn->conn != NULL)
		{
			free_resources_on_connection(conn->conn);

			rv = virConnectClose(conn->conn);
			if (rv!=0) {
				DPRINTF("%s: virConnectClose(%p) returned %d (%s)\n", __FUNCTION__, conn->conn, rv, LIBVIRT_G (last_error));
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virConnectClose failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virConnectClose(%p) completed successfully\n", __FUNCTION__, conn->conn);
				resource_change_counter(INT_RESOURCE_CONNECTION, NULL, conn->conn, 0);
			}
			conn->conn=NULL;
		}
		efree (conn);
	}
}

/* Destructor for domain resource */
static void php_libvirt_domain_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_domain *domain = (php_libvirt_domain*)rsrc->ptr;
	int rv = 0;
	
	if (domain != NULL)
	{
		if (domain->domain != NULL)
		{
			if (!check_resource_allocation(domain->conn->conn, INT_RESOURCE_DOMAIN, domain->domain)) {
				domain->domain=NULL;
				efree (domain);
				return;
			}

			rv = virDomainFree(domain->domain);
			if (rv != 0) {
				DPRINTF("%s: virDomainFree(%p) returned %d (%s)\n", __FUNCTION__, domain->domain, rv, LIBVIRT_G (last_error));
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virDomainFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virDomainFree(%p) completed successfully\n", __FUNCTION__, domain->domain);
				resource_change_counter(INT_RESOURCE_DOMAIN, domain->conn->conn, domain->domain, 0);
			}
			domain->domain=NULL;
		}
		efree (domain);
	}
}

/* Destructor for storagepool resource */
static void php_libvirt_storagepool_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_storagepool *pool = (php_libvirt_storagepool*)rsrc->ptr;
	int rv = 0;
	
	if (pool != NULL)
	{
		if (pool->pool != NULL)
		{
			if (!check_resource_allocation(NULL, INT_RESOURCE_STORAGEPOOL, pool->pool)) {
				pool->pool=NULL;
				efree(pool);
				return;
			}
			rv = virStoragePoolFree(pool->pool);
			if (rv!=0) {
				DPRINTF("%s: virStoragePoolFree(%p) returned %d (%s)\n", __FUNCTION__, pool->pool, rv, LIBVIRT_G (last_error));
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStoragePoolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virStoragePoolFree(%p) completed successfully\n", __FUNCTION__, pool->pool);
				resource_change_counter(INT_RESOURCE_STORAGEPOOL, NULL, pool->pool, 0);
			}
			pool->pool=NULL;
		}
		efree(pool);
	}
}

/* Destructor for volume resource */
static void php_libvirt_volume_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_volume *volume = (php_libvirt_volume*)rsrc->ptr;
	int rv = 0;
	
	if (volume != NULL)
	{
		if (volume->volume != NULL)
		{
			if (!check_resource_allocation(NULL, INT_RESOURCE_VOLUME, volume->volume)) {
				volume->volume=NULL;
				efree(volume);
				return;
			}
			rv = virStorageVolFree (volume->volume);
			if (rv!=0) {
				DPRINTF("%s: virStorageVolFree(%p) returned %d (%s)\n", __FUNCTION__, volume->volume, rv, LIBVIRT_G (last_error));
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStorageVolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virStorageVolFree(%p) completed successfully\n", __FUNCTION__, volume->volume);
				resource_change_counter(INT_RESOURCE_VOLUME, NULL, volume->volume, 0);
			}
			volume->volume=NULL;
		}
		efree(volume);
	}
}

/* Destructor for network resource */
static void php_libvirt_network_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_network *network = (php_libvirt_network*)rsrc->ptr;
	int rv = 0;
	
	if (network != NULL)
	{
		if (network->network != NULL)
		{
			if (!check_resource_allocation(NULL, INT_RESOURCE_CONNECTION, network->network)) {
				network->network=NULL;
				efree(network);
				return;
			}
			rv = virNetworkFree(network->network);
			if (rv!=0) {
				DPRINTF("%s: virNetworkFree(%p) returned %d (%s)\n", __FUNCTION__, network->network, rv, LIBVIRT_G (last_error));
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStorageVolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virNetworkFree(%p) completed successfully\n", __FUNCTION__, network->network);
				resource_change_counter(INT_RESOURCE_NETWORK, NULL, network->network, 0);
			}
			network->network=NULL;
		}
		efree(network);
	}
}

/* Destructor for nodedev resource */
static void php_libvirt_nodedev_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_nodedev *nodedev = (php_libvirt_nodedev*)rsrc->ptr;
	int rv = 0;
	
	if (nodedev != NULL)
	{
		if (nodedev->device != NULL)
		{
			if (!check_resource_allocation(nodedev->conn->conn, INT_RESOURCE_NODEDEV, nodedev->device)) {
				nodedev->device=NULL;
				efree(nodedev);
				return;
			}
			rv = virNodeDeviceFree(nodedev->device);
			if (rv!=0) {
				DPRINTF("%s: virNodeDeviceFree(%p) returned %d (%s)\n", __FUNCTION__, nodedev->device, rv, LIBVIRT_G (last_error));
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStorageVolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virNodeDeviceFree(%p) completed successfully\n", __FUNCTION__, nodedev->device);
				resource_change_counter(INT_RESOURCE_NODEDEV, nodedev->conn->conn, nodedev->device, 0);
			}
			nodedev->device=NULL;
		}
		efree(nodedev);
	}
}

/* Destructor for snapshot resource */
static void php_libvirt_snapshot_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_libvirt_snapshot *snapshot = (php_libvirt_snapshot*)rsrc->ptr;
	int rv = 0;
	
	if (snapshot != NULL)
	{
		if (snapshot->snapshot != NULL)
		{
			if (!check_resource_allocation(snapshot->domain->conn->conn, INT_RESOURCE_SNAPSHOT, snapshot->snapshot)) {
				snapshot->snapshot=NULL;
				efree(snapshot);
				return;
			}
			rv = virDomainSnapshotFree(snapshot->snapshot);
			if (rv!=0) {
				DPRINTF("%s: virDomainSnapshotFree(%p) returned %d\n", __FUNCTION__, snapshot->snapshot, rv);
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"virStorageVolFree failed with %i on destructor: %s", rv, LIBVIRT_G (last_error));
			}
			else {
				DPRINTF("%s: virDomainSnapshotFree(%p) completed successfully\n", __FUNCTION__, snapshot->snapshot);
				resource_change_counter(INT_RESOURCE_SNAPSHOT, snapshot->domain->conn->conn, snapshot->snapshot, 0);
			}
			snapshot->snapshot=NULL;
		}
		efree(snapshot);
	}
}

/* ZEND Module inicialization function */
PHP_MINIT_FUNCTION(libvirt)
{
	/* register resource types and their descriptors */
	le_libvirt_connection = zend_register_list_destructors_ex(php_libvirt_connection_dtor, NULL, PHP_LIBVIRT_CONNECTION_RES_NAME, module_number);
	le_libvirt_domain = zend_register_list_destructors_ex(php_libvirt_domain_dtor, NULL, PHP_LIBVIRT_DOMAIN_RES_NAME, module_number);
	le_libvirt_storagepool = zend_register_list_destructors_ex(php_libvirt_storagepool_dtor, NULL, PHP_LIBVIRT_STORAGEPOOL_RES_NAME, module_number);
	le_libvirt_volume = zend_register_list_destructors_ex(php_libvirt_volume_dtor, NULL, PHP_LIBVIRT_VOLUME_RES_NAME, module_number);
	le_libvirt_network = zend_register_list_destructors_ex(php_libvirt_network_dtor, NULL, PHP_LIBVIRT_NETWORK_RES_NAME, module_number);
	le_libvirt_nodedev = zend_register_list_destructors_ex(php_libvirt_nodedev_dtor, NULL, PHP_LIBVIRT_NODEDEV_RES_NAME, module_number);
	le_libvirt_snapshot = zend_register_list_destructors_ex(php_libvirt_snapshot_dtor, NULL, PHP_LIBVIRT_SNAPSHOT_RES_NAME, module_number);

	ZEND_INIT_MODULE_GLOBALS(libvirt, php_libvirt_init_globals, NULL);

	/* LIBVIRT CONSTANTS */

	/* XML contants */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_XML_SECURE", 	1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_XML_INACTIVE", 	2, CONST_CS | CONST_PERSISTENT);

	/* Domain constants */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_NOSTATE", 		0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_RUNNING", 		1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_BLOCKED", 		2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_PAUSED", 		3, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_SHUTDOWN", 		4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_SHUTOFF", 		5, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_CRASHED", 		6, CONST_CS | CONST_PERSISTENT);

	/* Domain snapshot constants */
	REGISTER_LONG_CONSTANT("VIR_SNAPSHOT_DELETE_CHILDREN",	 VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN,	CONST_CS | CONST_PERSISTENT);

	/* Memory constants */
	REGISTER_LONG_CONSTANT("VIR_MEMORY_VIRTUAL",		1, CONST_CS | CONST_PERSISTENT);

	/* Version checking constants */
	REGISTER_LONG_CONSTANT("VIR_VERSION_BINDING",           VIR_VERSION_BINDING,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_VERSION_LIBVIRT",           VIR_VERSION_LIBVIRT,    CONST_CS | CONST_PERSISTENT);
	
	/* Network constants */
	REGISTER_LONG_CONSTANT("VIR_NETWORKS_ACTIVE",		VIR_NETWORKS_ACTIVE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_NETWORKS_INACTIVE",		VIR_NETWORKS_INACTIVE,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_NETWORKS_ALL",		VIR_NETWORKS_ACTIVE |
								VIR_NETWORKS_INACTIVE,	CONST_CS | CONST_PERSISTENT);

	/* Credential constants */
	REGISTER_LONG_CONSTANT("VIR_CRED_USERNAME",		1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_CRED_AUTHNAME",		2, CONST_CS | CONST_PERSISTENT);
	/* RFC 1766 languages */
	REGISTER_LONG_CONSTANT("VIR_CRED_LANGUAGE",		3, CONST_CS | CONST_PERSISTENT);
	/* Client supplied a nonce */
	REGISTER_LONG_CONSTANT("VIR_CRED_CNONCE",		4, CONST_CS | CONST_PERSISTENT);
	/* Passphrase secret */
	REGISTER_LONG_CONSTANT("VIR_CRED_PASSPHRASE",		5, CONST_CS | CONST_PERSISTENT);
	/* Challenge response */
	REGISTER_LONG_CONSTANT("VIR_CRED_ECHOPROMPT",		6, CONST_CS | CONST_PERSISTENT);
	/* Challenge responce */
	REGISTER_LONG_CONSTANT("VIR_CRED_NOECHOPROMP",		7, CONST_CS | CONST_PERSISTENT);
	/* Authentication realm */
	REGISTER_LONG_CONSTANT("VIR_CRED_REALM",		8, CONST_CS | CONST_PERSISTENT);
	/* Externally managed credential More may be added - expect the unexpected */
	REGISTER_LONG_CONSTANT("VIR_CRED_EXTERNAL",		9, CONST_CS | CONST_PERSISTENT);

	/* Domain memory constants */
	/* The total amount of memory written out to swap space (in kB). */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_SWAP_IN",	0, CONST_CS | CONST_PERSISTENT);
	/*  Page faults occur when a process makes a valid access to virtual memory that is not available. */
	/* When servicing the page fault, if disk IO is * required, it is considered a major fault. If not, */
	/* it is a minor fault. * These are expressed as the number of faults that have occurred. */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_SWAP_OUT",	1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT",	2, CONST_CS | CONST_PERSISTENT);
	/* The amount of memory left completely unused by the system. Memory that is available but used for */
	/* reclaimable caches should NOT be reported as free. This value is expressed in kB. */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT",	3, CONST_CS | CONST_PERSISTENT);
	/* The total amount of usable memory as seen by the domain. This value * may be less than the amount */
	/* of memory assigned to the domain if a * balloon driver is in use or if the guest OS does not initialize */
	/* all * assigned pages. This value is expressed in kB.  */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_UNUSED",		4, CONST_CS | CONST_PERSISTENT);
	/* The number of statistics supported by this version of the interface. To add new statistics, add them */
	/* to the enum and increase this value. */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_AVAILABLE",	5, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_MEMORY_STAT_NR",		6, CONST_CS | CONST_PERSISTENT);
    
	/* Job constants */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_JOB_NONE",		0, CONST_CS | CONST_PERSISTENT);
	/* Job with a finite completion time */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_JOB_BOUNDED",	1, CONST_CS | CONST_PERSISTENT);
	/* Job without a finite completion time */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_JOB_UNBOUNDED",	2, CONST_CS | CONST_PERSISTENT);
	/* Job has finished but it's not cleaned up yet */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_JOB_COMPLETED",	3, CONST_CS | CONST_PERSISTENT);
	/* Job hit error but it's not cleaned up yet */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_JOB_FAILED",		4, CONST_CS | CONST_PERSISTENT);
	/* Job was aborted but it's not cleanup up yet */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_JOB_CANCELLED",	5, CONST_CS | CONST_PERSISTENT);

	/* Migration constants */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_LIVE",		  1, CONST_CS | CONST_PERSISTENT);
	/* direct source -> dest host control channel Note the less-common spelling that we're stuck with: */
	/* VIR_MIGRATE_TUNNELLED should be VIR_MIGRATE_TUNNELED */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_PEER2PEER",		  2, CONST_CS | CONST_PERSISTENT);
	/* tunnel migration data over libvirtd connection */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_TUNNELLED",		  4, CONST_CS | CONST_PERSISTENT);
	/* persist the VM on the destination */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_PERSIST_DEST",	  8, CONST_CS | CONST_PERSISTENT);
	/* undefine the VM on the source */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_UNDEFINE_SOURCE",	 16, CONST_CS | CONST_PERSISTENT);
	/* pause on remote side */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_PAUSED",		 32, CONST_CS | CONST_PERSISTENT);
	/* migration with non-shared storage with full disk copy */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_NON_SHARED_DISK",	 64, CONST_CS | CONST_PERSISTENT);
	/* migration with non-shared storage with incremental copy (same base image shared between source and destination) */
	REGISTER_LONG_CONSTANT("VIR_MIGRATE_NON_SHARED_INC",	128, CONST_CS | CONST_PERSISTENT);
    
    /* Modify device allocation based on current domain state */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_DEVICE_MODIFY_CURRENT",	0, CONST_CS | CONST_PERSISTENT);
	/* Modify live device allocation */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_DEVICE_MODIFY_LIVE",		1, CONST_CS | CONST_PERSISTENT);
	/* Modify persisted device allocation */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_DEVICE_MODIFY_CONFIG",	2, CONST_CS | CONST_PERSISTENT);
	/* Forcibly modify device (ex. force eject a cdrom) */
	REGISTER_LONG_CONSTANT("VIR_DOMAIN_DEVICE_MODIFY_FORCE",	4, CONST_CS | CONST_PERSISTENT);

	/* REGISTER_LONG_CONSTANT */
	REGISTER_LONG_CONSTANT("VIR_STORAGE_POOL_BUILD_NEW",	 	0, CONST_CS | CONST_PERSISTENT);
	/* Repair / reinitialize */
	REGISTER_LONG_CONSTANT("VIR_STORAGE_POOL_BUILD_REPAIR",	 	1, CONST_CS | CONST_PERSISTENT); 
	/* Extend existing pool */
	REGISTER_LONG_CONSTANT("VIR_STORAGE_POOL_BUILD_RESIZE",		2, CONST_CS | CONST_PERSISTENT);

	REGISTER_INI_ENTRIES();

	/* Initialize libvirt and set up error callback */
	virInitialize();

	void *thread_ctx = NULL;
	TSRMLS_SET_CTX(thread_ctx);
	virSetErrorFunc(thread_ctx, catch_error);

	return SUCCESS;
}

/* Zend module destruction */
PHP_MSHUTDOWN_FUNCTION(libvirt)
{
    UNREGISTER_INI_ENTRIES();

    /* return error callback back to default (outouts to STDOUT) */
    virSetErrorFunc(NULL, NULL);
    return SUCCESS;
}

/* Macros for obtaining resources from arguments */
#define GET_CONNECTION_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments");	\
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(conn, php_libvirt_connection*, &zconn, -1, PHP_LIBVIRT_CONNECTION_RES_NAME, le_libvirt_connection);\
	if ((conn==NULL) || (conn->conn==NULL)) RETURN_FALSE;\

#define GET_DOMAIN_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments"); \
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(domain, php_libvirt_domain*, &zdomain, -1, PHP_LIBVIRT_DOMAIN_RES_NAME, le_libvirt_domain);\
	if ((domain==NULL) || (domain->domain==NULL)) RETURN_FALSE;\

#define GET_NETWORK_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments");\
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(network, php_libvirt_network*, &znetwork, -1, PHP_LIBVIRT_NETWORK_RES_NAME, le_libvirt_network);\
	if ((network==NULL) || (network->network==NULL)) RETURN_FALSE;\

#define GET_NODEDEV_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments");\
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(nodedev, php_libvirt_nodedev*, &znodedev, -1, PHP_LIBVIRT_NODEDEV_RES_NAME, le_libvirt_nodedev);\
	if ((nodedev==NULL) || (nodedev->device==NULL)) RETURN_FALSE;\

#define GET_STORAGEPOOL_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments");\
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(pool, php_libvirt_storagepool*, &zpool, -1, PHP_LIBVIRT_STORAGEPOOL_RES_NAME, le_libvirt_storagepool);\
	if ((pool==NULL) || (pool->pool==NULL)) RETURN_FALSE;\

#define GET_VOLUME_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments");\
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(volume, php_libvirt_volume*, &zvolume, -1, PHP_LIBVIRT_VOLUME_RES_NAME, le_libvirt_volume);\
	if ((volume==NULL) || (volume->volume==NULL)) RETURN_FALSE;\

#define GET_SNAPSHOT_FROM_ARGS(args, ...) \
	reset_error();	\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, args, __VA_ARGS__) == FAILURE) {\
		set_error("Invalid arguments");\
		RETURN_FALSE;\
	}\
\
	ZEND_FETCH_RESOURCE(snapshot, php_libvirt_snapshot*, &zsnapshot, -1, PHP_LIBVIRT_SNAPSHOT_RES_NAME, le_libvirt_snapshot);\
	if ((snapshot==NULL) || (snapshot->snapshot==NULL)) RETURN_FALSE;\

/* Macro to "recreate" string with emalloc and free the original one */
#define RECREATE_STRING_WITH_E(str_out, str_in) \
str_out = estrndup(str_in, strlen(str_in)); \
	 free(str_in);	 \

#define LONGLONG_INIT \
	char tmpnumber[64];

#define LONGLONG_ASSOC(out,key,in) \
	if (LIBVIRT_G(longlong_to_string_ini)) { \
	  snprintf(tmpnumber,63,"%llu",in); \
          add_assoc_string_ex(out,key,strlen(key)+1,tmpnumber,1); \
        } \
	else \
	{ \
	   add_assoc_long(out,key,in); \
	}

#define LONGLONG_INDEX(out,key,in) \
	if (LIBVIRT_G(longlong_to_string_ini)) { \
	  snprintf(tmpnumber,63,"%llu",in); \
          add_index_string(out,key,tmpnumber,1); \
        } \
	else \
	{ \
           add_index_long(out, key,in); \
	}

/* Authentication callback function. Should receive list of credentials via cbdata and pass the requested one to libvirt */
static int libvirt_virConnectAuthCallback(virConnectCredentialPtr cred,  unsigned int ncred,  void *cbdata)
{
	int i,j;
	php_libvirt_cred_value *creds=(php_libvirt_cred_value*) cbdata;
	for(i=0;i<ncred;i++)
	{
		//printf ("Cred %i: type %i, prompt %s challenge %s\n",i,cred[i].type,cred[i].prompt,cred[i].challenge);
		if (creds != NULL)
			for (j=0;j<creds[0].count;j++)
			{
				if (creds[j].type==cred[i].type)
				{
					cred[i].resultlen=creds[j].resultlen;
					cred[i].result=malloc(creds[j].resultlen);
					strncpy(cred[i].result,creds[j].result,creds[j].resultlen);
				}
			}
			//printf ("Result: %s (%i)\n",cred[i].result,cred[i].resultlen);
	}

	return 0;
}

static int libvirt_virConnectCredType[] = {
	VIR_CRED_AUTHNAME,
	VIR_CRED_ECHOPROMPT,
	VIR_CRED_REALM,
	VIR_CRED_PASSPHRASE,
	VIR_CRED_NOECHOPROMPT,
	//VIR_CRED_EXTERNAL,
};

/* Common functions */

/*
	Function name:	libvirt_get_last_error
	Since version:	0.4.1(-1)
	Description:	This function is used to get the last error coming either from libvirt or the PHP extension itself
	Returns:	last error string
*/
PHP_FUNCTION(libvirt_get_last_error)
{
	if (LIBVIRT_G (last_error) == NULL) RETURN_NULL();
	RETURN_STRING(LIBVIRT_G (last_error),1);
}

/*
	Function name:	libvirt_connect
	Since version:	0.4.1(-1)
	Description:	libvirt_connect() is used to connect to the specified libvirt daemon using the specified URL, user can also set the readonly flag and/or set credentials for connection
	Arguments:	@url [string]: URI for connection
			@readonly [bool]: flag whether to use read-only connection or not
			@credentials [array]: array of connection credentials
	Returns:	libvirt connection resource
*/
PHP_FUNCTION(libvirt_connect)
{
	php_libvirt_connection *conn;
	php_libvirt_cred_value *creds=NULL;
	zval* zcreds=NULL;
	zval **data;
	int i;
	int j;
	int credscount=0;

	virConnectAuth libvirt_virConnectAuth= { libvirt_virConnectCredType, sizeof(libvirt_virConnectCredType)/sizeof(int), libvirt_virConnectAuthCallback, NULL};

	char *url=NULL;
	int url_len=0;
	int readonly=1;

	HashTable *arr_hash;
	HashPosition pointer;
	int array_count;

	char *key;
	unsigned int key_len;
	unsigned long index;

	unsigned long libVer;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sba", &url,&url_len,&readonly,&zcreds) == FAILURE) {
        	RETURN_FALSE;
	}

	if (virGetVersion(&libVer,NULL,NULL)!= 0)
		RETURN_FALSE;

	if (libVer<6002)
	{
		set_error("Only libvirt 0.6.2 and higher supported. Please upgrade your libvirt" TSRMLS_CC);
		RETURN_FALSE;
	}

	/* If 'null' value has been passed as URL override url to NULL value to autodetect the hypervisor */
	if (strcasecmp(url, "NULL") == 0)
		url = NULL;

	DPRINTF("%s: Connecting to %s ...\n", PHPFUNC, url);

	conn=emalloc(sizeof(php_libvirt_connection));
	if (zcreds==NULL)
	{	/* connecting without providing authentication */
		if (readonly)
			conn->conn = virConnectOpenReadOnly(url);
		else
			conn->conn = virConnectOpen(url);
	}
	else
	{  /* connecting with authentication (using callback) */
		arr_hash = Z_ARRVAL_P(zcreds);
		array_count = zend_hash_num_elements(arr_hash);

		credscount=array_count;
		creds=emalloc(credscount*sizeof(php_libvirt_cred_value));
		j=0;
		/* parse the input Array and create list of credentials. The list (array) is passed to callback function. */
		for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
			zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS;
			zend_hash_move_forward_ex(arr_hash, &pointer)) {
			    	if (Z_TYPE_PP(data) == IS_STRING) {
					if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
						PHPWRITE(key, key_len);
					} else {
						creds[j].type=index;
						creds[j].result=emalloc(Z_STRLEN_PP(data));
						creds[j].resultlen=Z_STRLEN_PP(data);
						strncpy(creds[j].result,Z_STRVAL_PP(data),Z_STRLEN_PP(data));
						j++;
					}
				}
		}
		creds[0].count=j;
		libvirt_virConnectAuth.cbdata = (void*)creds;
		conn->conn = virConnectOpenAuth (url, &libvirt_virConnectAuth, readonly ? VIR_CONNECT_RO : 0);
		for (i=0;i<creds[0].count;i++)
			efree(creds[i].result);
		efree(creds);
	}

	if (conn->conn == NULL)
	{
		efree (conn);
		RETURN_FALSE;
	}

	resource_change_counter(INT_RESOURCE_CONNECTION, NULL, conn->conn, 1);
	DPRINTF("%s: Connection to %s established, returning %p\n", PHPFUNC, url, conn->conn);

	ZEND_REGISTER_RESOURCE(return_value, conn, le_libvirt_connection);
	conn->resource_id=Z_LVAL_P(return_value);
} 

/*
	Function name:	libvirt_node_get_info
	Since version:	0.4.1(-1)
	Description:	Function is used to get the information about host node, mainly total memory installed, total CPUs installed and model information are useful
	Arguments:	@conn [resource]: resource for connection
	Returns:	array of node information or FALSE for error
*/
PHP_FUNCTION(libvirt_node_get_info)
{
	virNodeInfo info;
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int retval;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	retval=virNodeGetInfo	(conn->conn,&info);
	DPRINTF("%s: virNodeGetInfo returned %d\n", PHPFUNC, retval);
	if (retval==-1) RETURN_FALSE;

	array_init(return_value);
	add_assoc_string_ex(return_value, "model", 6, info.model, 1);
	add_assoc_long(return_value, "memory", (long)info.memory);
	add_assoc_long(return_value, "cpus", (long)info.cpus);
	add_assoc_long(return_value, "nodes", (long)info.nodes);
	add_assoc_long(return_value, "sockets", (long)info.sockets);
	add_assoc_long(return_value, "cores", (long)info.cores);
	add_assoc_long(return_value, "threads", (long)info.threads);
	add_assoc_long(return_value, "mhz", (long)info.mhz);
}

/*
	Function name:	libvirt_connect_get_information
	Since version:	0.4.1(-2)
	Description:	Function is used to get the information about the connection
	Arguments:	@conn [resource]: resource for connection
	Returns:	array of information about the connection
*/
PHP_FUNCTION(libvirt_connect_get_information)
{
	zval *zconn;
	char *tmp;
	unsigned long hvVer = 0;
	const char *type = NULL;
	char hvStr[64] = { 0 };
	int iTmp = -1;
	php_libvirt_connection *conn = NULL;

	GET_CONNECTION_FROM_ARGS("r",&zconn);
	
	tmp = virConnectGetURI(conn->conn);
	DPRINTF("%s: Got connection URI of %s...\n", PHPFUNC, tmp);
	array_init(return_value);
	add_assoc_string_ex(return_value, "uri", 4, tmp ? tmp : "unknown", 1);
	tmp = virConnectGetHostname(conn->conn);
	add_assoc_string_ex(return_value, "hostname", 9, tmp ? tmp : "unknown", 1);

	if ((virConnectGetVersion(conn->conn, &hvVer) == 0) && (type = virConnectGetType(conn->conn)))
	{
		add_assoc_string_ex(return_value, "hypervisor", 11, (char *)type, 1);
		add_assoc_long(return_value, "hypervisor_major",(long)((hvVer/1000000) % 1000));
		add_assoc_long(return_value, "hypervisor_minor",(long)((hvVer/1000) % 1000));
		add_assoc_long(return_value, "hypervisor_release",(long)(hvVer %1000));
		snprintf(hvStr, sizeof(hvStr), "%s %d.%d.%d", type,
					(long)((hvVer/1000000) % 1000), (long)((hvVer/1000) % 1000), (long)(hvVer %1000));
		add_assoc_string_ex(return_value, "hypervisor_string", 18, hvStr, 1);
	}
	
	add_assoc_long(return_value, "hypervisor_maxvcpus", virConnectGetMaxVcpus(conn->conn, type));
	iTmp = virConnectIsEncrypted(conn->conn);
	if (iTmp == 1)
		add_assoc_string_ex(return_value, "encrypted", 10, "Yes", 1);
	else
	if (iTmp == 0)
		add_assoc_string_ex(return_value, "encrypted", 10, "No", 1);
	else
		add_assoc_string_ex(return_value, "encrypted", 10, "unknown", 1);

	iTmp = virConnectIsSecure(conn->conn);
	if (iTmp == 1)
		add_assoc_string_ex(return_value, "secure", 7, "Yes", 1);
	else
	if (iTmp == 0)
		add_assoc_string_ex(return_value, "secure", 7, "No", 1);
	else
		add_assoc_string_ex(return_value, "secure", 7, "unknown", 1);
		
	add_assoc_long(return_value, "num_inactive_domains", virConnectNumOfDefinedDomains(conn->conn));
	add_assoc_long(return_value, "num_inactive_interfaces", virConnectNumOfDefinedInterfaces(conn->conn));
	add_assoc_long(return_value, "num_inactive_networks", virConnectNumOfDefinedNetworks(conn->conn));
	add_assoc_long(return_value, "num_inactive_storagepools", virConnectNumOfDefinedStoragePools(conn->conn));

	add_assoc_long(return_value, "num_active_domains", virConnectNumOfDomains(conn->conn));
	add_assoc_long(return_value, "num_active_interfaces", virConnectNumOfInterfaces(conn->conn));
	add_assoc_long(return_value, "num_active_networks", virConnectNumOfNetworks(conn->conn));
	add_assoc_long(return_value, "num_active_storagepools", virConnectNumOfStoragePools(conn->conn));

	add_assoc_long(return_value, "num_total_domains", virConnectNumOfDomains(conn->conn) + virConnectNumOfDefinedDomains(conn->conn));
	add_assoc_long(return_value, "num_total_interfaces", virConnectNumOfInterfaces(conn->conn) + virConnectNumOfDefinedInterfaces(conn->conn));
	add_assoc_long(return_value, "num_total_networks", virConnectNumOfNetworks(conn->conn) + virConnectNumOfDefinedNetworks(conn->conn));
	add_assoc_long(return_value, "num_total_storagepools", virConnectNumOfStoragePools(conn->conn) +  virConnectNumOfDefinedStoragePools(conn->conn));

	add_assoc_long(return_value, "num_secrets", virConnectNumOfSecrets(conn->conn));
	add_assoc_long(return_value, "num_nwfilters", virConnectNumOfNWFilters(conn->conn));
}

/*
	Function name:	libvirt_connect_get_uri
	Since version:	0.4.1(-1)
	Description:	Function is used to get the connection URI. This is useful to check the hypervisor type of host machine when using "null" uri to libvirt_connect()
	Arguments:	@conn [resource]: resource for connection
	Returns:	connection URI string or FALSE for error
*/
PHP_FUNCTION(libvirt_connect_get_uri)
{
	zval *zconn;
	char *uri;
	char *uri_out;
	php_libvirt_connection *conn = NULL;

	GET_CONNECTION_FROM_ARGS("r",&zconn);
	uri = virConnectGetURI(conn->conn);
	DPRINTF("%s: virConnectGetURI returned %s\n", PHPFUNC, uri);
	if (uri == NULL) RETURN_FALSE;

	RECREATE_STRING_WITH_E(uri_out, uri);
	RETURN_STRING(uri_out, 0);
}

/*
	Function name:	libvirt_connect_get_hostname
	Since version:	0.4.1(-1)
	Description:	Function is used to get the hostname of the guest associated with the connection
	Arguments:	@conn [resource]: resource for connection
	Returns:	hostname of the host node or FALSE for error
*/
PHP_FUNCTION(libvirt_connect_get_hostname)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	char *hostname;
	char *hostname_out;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	hostname=virConnectGetHostname(conn->conn);
	DPRINTF("%s: virConnectGetHostname returned %s\n", PHPFUNC, hostname);
	if (hostname==NULL) RETURN_FALSE;

	RECREATE_STRING_WITH_E(hostname_out,hostname);

	RETURN_STRING(hostname_out,0);
}

/*
	Function name:	libvirt_connect_get_hypervisor
	Since version:	0.4.1(-2)
	Description:	Function is used to get the information about the hypervisor on the connection identified by the connection pointer
	Arguments:	@conn [resource]: resource for connection
	Returns:	array of hypervisor information if available
*/
PHP_FUNCTION(libvirt_connect_get_hypervisor)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	unsigned long hvVer = 0;
	const char *type = NULL;
	char hvStr[64] = { 0 };

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	if (virConnectGetVersion(conn->conn, &hvVer) != 0)
		RETURN_FALSE;

	type = virConnectGetType(conn->conn);
	if (type == NULL)
		RETURN_FALSE;

	DPRINTF("%s: virConnectGetType returned %s\n", PHPFUNC, type);

	array_init(return_value);
	add_assoc_string_ex(return_value, "hypervisor", 11, (char *)type, 1);
	add_assoc_long(return_value, "major",(long)((hvVer/1000000) % 1000));
	add_assoc_long(return_value, "minor",(long)((hvVer/1000) % 1000));
	add_assoc_long(return_value, "release",(long)(hvVer %1000));

	snprintf(hvStr, sizeof(hvStr), "%s %d.%d.%d", type,
				(long)((hvVer/1000000) % 1000), (long)((hvVer/1000) % 1000), (long)(hvVer %1000));
	add_assoc_string_ex(return_value, "hypervisor_string", 18, hvStr, 1);
}

/*
	Function name:	libvirt_connect_is_encrypted
	Since version:	0.4.1(-2)
	Description:	Function is used to get the information whether the connection is encrypted or not
	Arguments:	@conn [resource]: resource for connection
	Returns:	1 if encrypted, 0 if not encrypted, -1 on error
*/
PHP_FUNCTION(libvirt_connect_get_encrypted)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	RETURN_LONG( virConnectIsEncrypted(conn->conn) );
}


/*
	Function name:	libvirt_connect_is_secure
	Since version:	0.4.1(-2)
	Description:	Function is used to get the information whether the connection is secure or not
	Arguments:	@conn [resource]: resource for connection
	Returns:	1 if secure, 0 if not secure, -1 on error
*/
PHP_FUNCTION(libvirt_connect_get_secure)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	RETURN_LONG( virConnectIsSecure(conn->conn) );
}

/*
	Function name:	libvirt_connect_get_maxvcpus
	Since version:	0.4.1(-2)
	Description:	Function is used to get maximum number of VCPUs per VM on the hypervisor connection
	Arguments:	@conn [resource]: resource for connection
	Returns:	number of VCPUs available per VM on the connection or FALSE for error
*/
PHP_FUNCTION(libvirt_connect_get_maxvcpus)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	const char *type = NULL;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	type = virConnectGetType(conn->conn);
	if (type == NULL)
		RETURN_FALSE;

	RETURN_LONG(virConnectGetMaxVcpus(conn->conn, type));
}

/*
	Function name:	libvirt_connect_get_capabilities
	Since version:	0.4.1(-2)
	Description:	Function is used to get the capabilities information from the connection
	Arguments:	@conn [resource]: resource for connection
	Returns:	capabilities XML from the connection or FALSE for error
*/
PHP_FUNCTION(libvirt_connect_get_capabilities)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	char *caps;
	char *caps_out;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	caps = virConnectGetCapabilities(conn->conn);
	if (caps == NULL)
		RETURN_FALSE;

	RECREATE_STRING_WITH_E(caps_out, caps);

	RETURN_STRING(caps_out,0);
}

/*
	Function name:	libvirt_connect_get_sysinfo
	Since version:	0.4.1(-2)
	Description:	Function is used to get the system information from connection if available
	Arguments:	@conn [resource]: resource for connection
	Returns:	XML description of system information from the connection or FALSE for error
*/
#if LIBVIR_VERSION_NUMBER>=8008
PHP_FUNCTION(libvirt_connect_get_sysinfo)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	char *sysinfo;
	char *sysinfo_out;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	sysinfo=virConnectGetSysinfo(conn->conn, 0);
	if (sysinfo==NULL) RETURN_FALSE;

	RECREATE_STRING_WITH_E(sysinfo_out, sysinfo);

	RETURN_STRING(sysinfo_out,0);
}
#else
PHP_FUNCTION(libvirt_connect_get_sysinfo)
{
	set_error("Only libvirt 0.8.8 or higher supports virConnectGetSysinfo() API function");
	RETURN_FALSE;
}
#endif

/*
	Private function name:	get_string_from_xpath
	Since version:		0.4.1(-1)
	Description:		Function is used to get the XML xPath expression from the XML document. This can be added to val array if not NULL.
	Arguments:		@xml [string]: input XML document
				@xpath [string]: xPath expression to find nodes in the XML document
				@val [array]: Zend array resource to put data to
				@retVal [int]: return value of the parsing
	Returns:		string containing data of last match found
*/
char *get_string_from_xpath(char *xml, char *xpath, zval **val, int *retVal)
{
	xmlParserCtxtPtr xp;
	xmlDocPtr doc;
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	xmlNodeSetPtr nodeset;
	int ret = 0, i;
	char *value = NULL; 
	char key[8] = { 0 };

	if ((xpath == NULL) || (xml == NULL))
	{
		return NULL;
	}

	xp = xmlCreateDocParserCtxt( (xmlChar *)xml );
	if (!xp) {
		if (retVal)
			*retVal = -1;
		return NULL;
	}
	doc = xmlCtxtReadDoc(xp, (xmlChar *)xml, NULL, NULL, 0);
	if (!doc) {
		if (retVal)
			*retVal = -2;
		xmlCleanupParser();
		return NULL;
	}

	context = xmlXPathNewContext(doc);
	if (!context) {
		if (retVal)
			*retVal = -3;
		xmlCleanupParser();
		return NULL;
	}

	result = xmlXPathEvalExpression( (xmlChar *)xpath, context);
	if (!result) {
		if (retVal)
			*retVal = -4;
	        xmlXPathFreeContext(context);
		xmlCleanupParser();
        	return NULL;
	}

	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		xmlXPathFreeContext(context);
		xmlCleanupParser();
		if (retVal)
			*retVal = 0;
		return NULL;
	}

	nodeset = result->nodesetval;
	ret = nodeset->nodeNr;

	if (ret == 0) {
		xmlXPathFreeObject(result);
		xmlFreeDoc(doc);
		xmlXPathFreeContext(context);
		xmlCleanupParser();
		if (retVal)
			*retVal = 0;
		return NULL;
	}

	if (val != NULL) {
		ret = 0;
		for (i = 0; i < nodeset->nodeNr; i++) {
			if (xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1) != NULL) {
				value = (char *)xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);

				snprintf(key, sizeof(key), "%d", i);
				add_assoc_string_ex(*val, key, strlen(key)+1, value, 1);
				ret++;
			}
		}
		add_assoc_long(*val, "num", (long)ret);
	}
	else {
		if (xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1) != NULL)
			value = (char *)xmlNodeListGetString(doc, nodeset->nodeTab[0]->xmlChildrenNode, 1);
	}

	xmlXPathFreeContext(context);
	xmlXPathFreeObject(result);
	xmlFreeDoc(doc);
	xmlCleanupParser();

	if (retVal)
		*retVal = ret;

	return (value != NULL) ? strdup(value) : NULL;
}

/*
	Private function name:	dec_to_bin
	Since version:		0.4.1(-1)
	Description:		Function dec_to_bin() converts the unsigned long long decimal (used e.g. for IPv4 address) to it's binary representation
	Arguments:		@decimal [int]: decimal value to be converted to binary interpretation
				@binary [string]: output binary string with the binary interpretation
	Returns:		None
*/
void dec_to_bin(unsigned long long decimal, char *binary)
{
	int  k = 0, n = 0;
	int  neg_flag = 0;
	int  remain;
	int  old_decimal;
	char temp[128] = { 0 };

	if (decimal < 0)
	{      
		decimal = -decimal;
		neg_flag = 1;
	}
	do 
	{
		old_decimal = decimal;
		remain    = decimal % 2;
		decimal   = decimal / 2;
		temp[k++] = remain + '0';
	} while (decimal > 0);

	if (neg_flag)
		temp[k++] = '-';
	else
		temp[k++] = ' ';

	while (k >= 0)
		binary[n++] = temp[--k];

	binary[n-1] = 0;
}

/*
	Private function name:	get_subnet_bits
	Since version:		0.4.1(-1)
	Description:		Function is used to get number of bits used by subnet determined by IP. Useful to get the CIDR IPv4 address representation
	Arguments:		@ip [string]: IP address to calculate subnet bits from
	Returns:		number of bits used by subnet mask
*/
int get_subnet_bits(char *ip)
{
	char tmp[4] = { 0 };
	int i, part = 0, ii = 0, skip = 0;
	unsigned long long retval = 0;
	char *binary;
	int maxBits = 64;

	for (i = 0; i < strlen(ip); i++) {
		if (ip[i] == '.') {
			ii = 0;
			retval += (atoi(tmp) * pow(256, 3 - part));
			part++;
			memset(tmp, 0, 4);
		}
		else {
			tmp[ii++] = ip[i];
		}
	}

	retval += (atoi(tmp) * pow(256, 3 - part));
	binary = (char *)malloc( maxBits * sizeof(char) );
	dec_to_bin(retval, binary);

	for (i = 0; i < strlen(binary); i++) {
		if ((binary[i] != '1') && (binary[i] != '0'))
			skip++;
		else
		if (binary[i] != '1')
			break;
	}
	free(binary);

	return i - skip;
}

/*
	Private function name:	get_next_free_numeric_value
	Since version:		0.4.2
	Description:		Function is used to get the next free slot to be used for adding new NIC device or others
	Arguments:		@res [virDomainPtr]: standard libvirt domain pointer identified by virDomainPtr
				@xpath [string]: xPath expression of items to get the next free value of
	Returns:		next free numeric value
*/
long get_next_free_numeric_value(virDomainPtr domain, char *xpath)
{
	zval *output = NULL;
	char *xml;
	int retval = -1;
	HashTable *arr_hash;
	HashPosition pointer;
	int array_count;
	zval **data;
	char *key;
	unsigned int key_len;
	unsigned long index;
	long max_slot = -1;

	xml=virDomainGetXMLDesc(domain, VIR_DOMAIN_XML_INACTIVE);
	output = emalloc( sizeof(zval) );
	array_init(output);
	free( get_string_from_xpath(xml, xpath, &output, &retval) );

	arr_hash = Z_ARRVAL_P(output);
	array_count = zend_hash_num_elements(arr_hash);
	for (zend_hash_internal_pointer_reset_ex(arr_hash, &pointer);
			zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS;
			zend_hash_move_forward_ex(arr_hash, &pointer)) {
			if (Z_TYPE_PP(data) == IS_STRING) {
				if (zend_hash_get_current_key_ex(arr_hash, &key, &key_len, &index, 0, &pointer) != HASH_KEY_IS_STRING) {
					unsigned int num = -1;

					sscanf(Z_STRVAL_PP(data), "%x", &num);
					if (num > max_slot)
						max_slot = num;
				}
		}
	}

	efree(output);
	return max_slot + 1;
}

/* Domain functions */

/*
	Function name:	libvirt_domain_get_counts
	Since version:	0.4.1(-1)
	Description:	Function is getting domain counts for all, active and inactive domains
	Arguments:	@conn [resource]: libvirt connection resource from libvirt_connect()
	Returns:	array of total, active and inactive (but defined) domain counts
*/
PHP_FUNCTION(libvirt_domain_get_counts)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count_defined;
	int count_active;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	count_defined = virConnectNumOfDefinedDomains (conn->conn);
	count_active  = virConnectNumOfDomains (conn->conn);

	array_init(return_value);
	add_assoc_long(return_value, "total", (long)(count_defined + count_active));
	add_assoc_long(return_value, "active", (long)count_active);
	add_assoc_long(return_value, "inactive", (long)count_defined);
}

/*
	Function name:	libvirt_domain_get_autostart
	Since version:	0.4.1(-1)
	Description:	Function is getting the autostart value for the domain
	Arguments:	@res [resource]: libvirt domain resource
	Returns:	autostart value or -1
*/
PHP_FUNCTION(libvirt_domain_get_autostart)
{
	php_libvirt_domain *domain = NULL;
	zval *zdomain;
	int flags = 0;

	GET_DOMAIN_FROM_ARGS ("r", &zdomain);

	if (virDomainGetAutostart (domain->domain, &flags) != 0)
	{
		RETURN_LONG (-1);
	}
	RETURN_LONG ((long)flags);
}

/*
	Function name:	libvirt_domain_set_autostart
	Since version:	0.4.1(-1)
	Description:	Function is setting the autostart value for the domain
	Arguments:	@res [resource]: libvirt domain resource
			@flags [int]: flag to enable/disable autostart
	Returns:	TRUE on success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_set_autostart)
{
	php_libvirt_domain *domain = NULL;
	zval *zdomain;
	zend_bool flags = 0;

	GET_DOMAIN_FROM_ARGS ("rb", &zdomain, &flags);

	if (virDomainSetAutostart (domain->domain, flags) != 0)
	{
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_is_active
	Since version:	0.4.1(-1)
	Description:	Function is getting information whether domain identified by resource is active or not
	Arguments:	@res [resource]: libvirt domain resource
	Returns:	virDomainIsActive() result on the domain
*/
PHP_FUNCTION(libvirt_domain_is_active)
{
	php_libvirt_domain *domain = NULL;
	zval *zdomain;

	GET_DOMAIN_FROM_ARGS ("r", &zdomain);

	RETURN_LONG (virDomainIsActive(domain->domain));
}

/*
	Function name:	libvirt_domain_lookup_by_name
	Since version:	0.4.1(-1)
	Description:	Function is used to lookup for domain by it's name
	Arguments:	@res [resource]: libvirt connection resource from libvirt_connect()
			@name [string]: domain name to look for
	Returns:	libvirt domain resource
*/
PHP_FUNCTION(libvirt_domain_lookup_by_name)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int name_len;
	char *name=NULL;
	virDomainPtr domain=NULL;
	php_libvirt_domain *res_domain;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&name,&name_len);
	if ( (name == NULL) || (name_len<1)) RETURN_FALSE;
	domain=virDomainLookupByName(conn->conn,name);
	if (domain==NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = domain;
	res_domain->conn = conn;

	DPRINTF("%s: domain name = '%s', returning %p\n", PHPFUNC, name, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_lookup_by_uuid
	Since version:	0.4.1(-1)
	Description:	Function is used to lookup for domain by it's UUID in the binary format
	Arguments:	@res [resource]: libvirt connection resource from libvirt_connect()
			@uuid [string]: binary defined UUID to look for
	Returns:	libvirt domain resource
*/
PHP_FUNCTION(libvirt_domain_lookup_by_uuid)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int uuid_len;
	unsigned char *uuid=NULL;
	virDomainPtr domain=NULL;
	php_libvirt_domain *res_domain;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&uuid,&uuid_len);

	if ( (uuid == NULL) || (uuid_len<1)) RETURN_FALSE;
	domain=virDomainLookupByUUID(conn->conn,uuid);
	if (domain==NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = domain;
	res_domain->conn=conn;

	DPRINTF("%s: domain UUID = '%s', returning %p\n", PHPFUNC, uuid, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_lookup_by_uuid_string
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain by it's UUID that's accepted in string format
	Arguments:	@res [resource]: libvirt connection resource from libvirt_connect()
			@uuid [string]: domain UUID [in string format] to look for
	Returns:	libvirt domain resource
*/
PHP_FUNCTION(libvirt_domain_lookup_by_uuid_string)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int uuid_len;
	char *uuid=NULL;
	virDomainPtr domain=NULL;
	php_libvirt_domain *res_domain;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&uuid,&uuid_len);

	if ( (uuid == NULL) || (uuid_len<1)) RETURN_FALSE;
	domain=virDomainLookupByUUIDString(conn->conn,uuid);
	if (domain==NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = domain;

	res_domain->conn=conn;

	DPRINTF("%s: domain UUID string = '%s', returning %p\n", PHPFUNC, uuid, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_lookup_by_id
	Since version:	0.4.1(-1)
	Description:	Function is used to get domain by it's ID, applicable only to running guests
	Arguments:	@conn [resource]: libvirt connection resource from libvirt_connect()
			@id   [string]: domain id to look for
	Returns:	libvirt domain resource
*/
PHP_FUNCTION(libvirt_domain_lookup_by_id)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	long id;
	virDomainPtr domain=NULL;
	php_libvirt_domain *res_domain;

	GET_CONNECTION_FROM_ARGS("rl",&zconn,&id);

	domain=virDomainLookupByID(conn->conn,(int)id);
	if (domain==NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = domain;
	res_domain->conn=conn;

	DPRINTF("%s: domain id = '%d', returning %p\n", PHPFUNC, (int)id, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_get_name
	Since version:	0.4.1(-1)
	Description:	Function is used to get domain name from it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	domain name string
*/
PHP_FUNCTION(libvirt_domain_get_name)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	const char *name=NULL;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	if (domain->domain == NULL)
		RETURN_FALSE;

	name=virDomainGetName(domain->domain);
	DPRINTF("%s: virDomainGetName(%p) returned %s\n", PHPFUNC, domain->domain, name);
	if (name==NULL) RETURN_FALSE;

	RETURN_STRING(name, 1);  //we can use the copy mechanism as we need not to free name (we even can not!)
}

/*
	Function name:	libvirt_domain_get_uuid_string
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's UUID in string format
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	domain UUID string
*/
PHP_FUNCTION(libvirt_domain_get_uuid_string)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *uuid;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	uuid=emalloc(VIR_UUID_STRING_BUFLEN);
	retval=virDomainGetUUIDString(domain->domain,uuid);
	DPRINTF("%s: virDomainGetUUIDString(%p) returned %d (%s)\n", PHPFUNC, domain->domain, retval, uuid);
	if (retval!=0) RETURN_FALSE;

	RETURN_STRING(uuid,0);
}

/*
	Function name:	libvirt_domain_get_screenshot
	Since version:	0.4.2
	Description:	Function uses gvnccapture (if available) to get the screenshot of the running domain
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@server [string]: server string for the host machine
			@scancode [int]: integer value of the scancode to be send to refresh screen
	Returns:	PNG image binary data
*/
PHP_FUNCTION(libvirt_domain_get_screenshot)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	pid_t childpid = -1;
	pid_t w = -1;
	int retval = -1;
	int fd = -1, fsize = -1;
	char file[] = "/tmp/libvirt-php-tmp-XXXXXX";
	char *buf = NULL;
	char *tmp = NULL;
	char *xml = NULL;
	int port = -1;
	char *hostname = NULL;
	int hostname_len;
	int scancode = 10;
	char *path;
	char name[1024] = { 0 };

	path = get_feature_binary("screenshot");
	DPRINTF("%s: get_feature_binary('screenshot') returned %s\n", PHPFUNC, path);

	if (access(path, X_OK) != 0) {
		set_error("Cannot find gvnccapture binary");
		RETURN_FALSE;
	}
	
	GET_DOMAIN_FROM_ARGS("rs|l",&zdomain, &hostname, &hostname_len, &scancode);

	xml=virDomainGetXMLDesc(domain->domain, 0);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

 	tmp = get_string_from_xpath(xml, "//domain/devices/graphics/@port", NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		set_error("Cannot get the VNC port");
		RETURN_FALSE;
	}
	
	vnc_refresh_screen(hostname, tmp);

	port = atoi(tmp)-5900;
	
	if (mkstemp(file) == 0)
		RETURN_FALSE;

	/* Get the current hostname and override to localhost if local machine */
	gethostname(name, 1024);
	if (strcmp(name, hostname) == 0)
		hostname = strdup("localhost");

	DPRINTF("%s: Getting screenshot of %s:%d to temporary file %s\n", PHPFUNC, hostname, port, file);

	childpid = fork();
	if (childpid == -1)
		RETURN_FALSE;
	
	if (childpid == 0) {
		char tmpp[64] = { 0 };
		
		snprintf(tmpp, sizeof(tmpp), "%s:%d", hostname, port);
		retval = execlp(path, basename(path), tmpp, file, NULL);
		_exit( retval );
	}
	else {
		do {
			w = waitpid(childpid, &retval, 0);
			if (w == -1)
				RETURN_FALSE;
		} while (!WIFEXITED(retval) && !WIFSIGNALED(retval));		
	}
	
	if (WEXITSTATUS(retval) != 0) {
		set_error("Cannot spawn utility to get screenshot");
		RETURN_FALSE;
	}
	
	fd = open(file, O_RDONLY);
	fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	buf = emalloc( (fsize + 1) * sizeof(char) );
	memset(buf, 0, fsize + 1);
	if (read(fd, buf, fsize) < 0) {
		close(fd);
		unlink(file);
		RETURN_FALSE;
	}
	close(fd);
	
	if (access(file, F_OK) == 0) {
		DPRINTF("%s: Temporary file %s deleted\n", PHPFUNC, file);
		unlink(file);
	}

	/* This is necessary to make the output binary safe */
	Z_STRLEN_P(return_value) = fsize;
	Z_STRVAL_P(return_value) = buf;
	Z_TYPE_P(return_value) = IS_STRING;
}

/*
	Function name:	libvirt_domain_send_keys
	Since version:	0.4.2
	Description:	Function sends keys to the domain's VNC window
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@server [string]: server string of the host machine
			@scancode [int]: integer scancode to be sent to VNC window
	Returns:	TRUE on success, FALSE otherwise
*/
PHP_FUNCTION(libvirt_domain_send_keys)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval = -1;
	char *tmp = NULL;
	char *xml = NULL;
	char *hostname = NULL;
	int hostname_len;
	char *keys = NULL;
	int keys_len;
	int i, ret = 0;

	GET_DOMAIN_FROM_ARGS("rss",&zdomain, &hostname, &hostname_len, &keys, &keys_len);

	DPRINTF("%s: Sending %d VNC keys to %s...\n", PHPFUNC, strlen(keys), hostname);

	xml=virDomainGetXMLDesc(domain->domain, 0);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

 	tmp = get_string_from_xpath(xml, "//domain/devices/graphics/@port", NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		set_error("Cannot get the VNC port");
		RETURN_FALSE;
	}

	DPRINTF("%s: About to send string '%s' (%d keys) to %s:%s\n", PHPFUNC, keys, strlen(keys), hostname, tmp);

	for (i = 0; i < strlen(keys); i += 4) {
		char keyseq[5] = { 0 };

		keyseq[0] = keys[i];
		keyseq[1] = strlen(keys) >= i + 1 ? keys[i + 1] : 0;
		keyseq[2] = strlen(keys) >= i + 2 ? keys[i + 2] : 0;
		keyseq[3] = strlen(keys) >= i + 3 ? keys[i + 3] : 0;
		keyseq[4] = 0;

		if (keyseq[3] == '\\') {
			i--;
			keyseq[3] = 0;
		}

		ret = vnc_send_keys(hostname, tmp, keyseq);
		DPRINTF("%s: Sequence sending result is %d\n", PHPFUNC, ret);
	}

	if (ret == 0) {
		RETURN_TRUE
	}
	else {
		char tmpp[64] = { 0 };
		snprintf(tmpp, sizeof(tmpp), "Cannot send keys, error code %d", ret);
		set_error(tmpp);
		RETURN_FALSE;
	}
}

/*
	Function name:	libvirt_domain_send_pointer_event
	Since version:	0.4.2
	Description:	Function sends keys to the domain's VNC window
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@server [string]: server string of the host machine
			@pos_x [int]: position on x-axis
			@pos_y [int]: position on y-axis
			@clicked [int]: mask of clicked buttons (0 for none, bit 1 for button #1, bit 8 for button #8)
			@release [int]: boolean value (0 or 1) whether to release the buttons automatically once pressed
	Returns:	TRUE on success, FALSE otherwise
*/
PHP_FUNCTION(libvirt_domain_send_pointer_event)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval = -1;
	char *tmp = NULL;
	char *xml = NULL;
	char *hostname = NULL;
	int hostname_len;
	int pos_x = 0;
	int pos_y = 0;
	int clicked = 0;
	int release = 1;
	int ret;

	GET_DOMAIN_FROM_ARGS("rslll|b",&zdomain, &hostname, &hostname_len, &pos_x, &pos_y, &clicked, &release);

	xml=virDomainGetXMLDesc(domain->domain, 0);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

 	tmp = get_string_from_xpath(xml, "//domain/devices/graphics/@port", NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		set_error("Cannot get the VNC port");
		RETURN_FALSE;
	}

	DPRINTF("%s: x = %d, y = %d, clicked = %d, release = %d, hostname = %s...\n", PHPFUNC, pos_x, pos_y, clicked, release, hostname);
	ret = vnc_send_pointer_event(hostname, tmp, pos_x, pos_y, clicked, release);
	if (ret == 0) {
		DPRINTF("%s: Pointer event result is %d\n", PHPFUNC, ret);
		RETURN_TRUE
	}
	else {
		char error[1024] = { 0 };
		snprintf(error, sizeof(error), "Cannot send pointer event, error code = %d (%s)", ret, strerror(-ret));
		set_error(error);
		RETURN_FALSE;
	}
}

/*
	Function name:	libvirt_domain_get_uuid
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's UUID in binary format
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	domain UUID in binary format
*/
PHP_FUNCTION(libvirt_domain_get_uuid)
{

	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *uuid;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	uuid=emalloc(VIR_UUID_BUFLEN);
	retval=virDomainGetUUID(domain->domain,(unsigned char *)uuid);
	DPRINTF("%s: virDomainGetUUID(%p, %p) returned %d\n", PHPFUNC, domain->domain, uuid, retval);
	if (retval!=0) RETURN_FALSE;

	RETURN_STRING(uuid,0);
}

/*
	Function name:	libvirt_domain_get_id
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's ID, applicable to running guests only
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	running domain ID or -1 if not running
*/
PHP_FUNCTION(libvirt_domain_get_id)
{

	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainGetID(domain->domain);
	DPRINTF("%s: virDomainGetID(%p) returned %d\n", PHPFUNC, domain->domain, retval);

	RETURN_LONG(retval);
}

/*
	Function name:	libvirt_domain_get_next_dev_ids
	Since version:	0.4.2
	Description:	This functions can be used to get the next free slot if you intend to add a new device identified by slot to the domain, e.g. NIC device
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	next free slot number for the domain
*/
PHP_FUNCTION(libvirt_domain_get_next_dev_ids)
{
	long dom, bus, slot, func;
        php_libvirt_domain *domain=NULL;
        zval *zdomain;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	DPRINTF("%s: Getting the next dev ids for domain %p\n", PHPFUNC, domain->domain);

	dom = get_next_free_numeric_value(domain->domain, "//@domain");
	bus = get_next_free_numeric_value(domain->domain, "//@bus");
	slot = get_next_free_numeric_value(domain->domain, "//@slot");
	func = get_next_free_numeric_value(domain->domain, "//@func");

	array_init(return_value);
	add_assoc_long(return_value, "next_domain", dom);
	add_assoc_long(return_value, "next_bus", bus);
	add_assoc_long(return_value, "next_slot", slot);
	add_assoc_long(return_value, "next_func", func);
}

/*
	Function name:	libvirt_domain_get_xml_desc
	Since version:	0.4.1(-1), changed 0.4.2
	Description:	Function is used to get the domain's XML description
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@xpath [string]: optional xPath expression string to get just this entry, can be NULL
	Returns:	domain XML description string or result of xPath expression
*/
PHP_FUNCTION(libvirt_domain_get_xml_desc)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmp = NULL;
	char *xml;
	char *xml_out;
	char *xpath = NULL;
	int xpath_len;
	long flags=0;
	int retval = -1;

	GET_DOMAIN_FROM_ARGS("rs|l",&zdomain,&xpath,&xpath_len,&flags);
	if (xpath_len < 1)
		xpath = NULL;

	DPRINTF("%s: Getting the XML for domain %p (xPath = %s)\n", PHPFUNC, domain->domain, xpath);

	xml=virDomainGetXMLDesc(domain->domain,flags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

 	tmp = get_string_from_xpath(xml, xpath, NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		RECREATE_STRING_WITH_E (xml_out, xml);
	} else {
		RECREATE_STRING_WITH_E (xml_out, tmp);
	}

	RETURN_STRING(xml_out,0);
}

/*
	Function name:	libvirt_domain_change_vcpus
	Since version:	0.4.2
	Description:	Function is used to change the VCPU count for the domain
	Arguments:	@res [resource]: libvirt domain resource
			@numCpus [int]: number of VCPUs to be set for the guest
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_change_vcpus)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	int numCpus = 1;
	int retval = -1;
	int pos = -1;
	php_libvirt_domain *res_domain = NULL;
	php_libvirt_connection *conn   = NULL;
	virDomainPtr dom=NULL;

	GET_DOMAIN_FROM_ARGS("rl|l",&zdomain,&numCpus,&xflags);

	DPRINTF("%s: Changing domain vcpu count to %d, domain = %p\n", PHPFUNC, numCpus, domain->domain);

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "  <vcpu>%d</vcpu>\n", numCpus);
	tmp1 = strstr(xml, "</vcpu>") + strlen("</vcpu>");
	pos = strlen(xml) - strlen(tmp1);

	tmp2 = emalloc( ( pos + 1 )* sizeof(char) );
	memset(tmp2, 0, pos + 1);
	memcpy(tmp2, xml, pos - 15);

	new_len = strlen(tmp1) + strlen(tmp2) + strlen(new) + 2;
	new_xml = emalloc( new_len * sizeof(char) );
	snprintf(new_xml, new_len, "%s\n%s%s", tmp2, new, tmp1);

	conn = domain->conn;
	
	virDomainUndefine(domain->domain);
	retval = virDomainFree(domain->domain);
	if (retval != 0) {
		DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
	else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
		DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn, new_xml);
	if (dom==NULL) {
		DPRINTF("%s: Function failed, restoring original XML\n", PHPFUNC);
		dom=virDomainDefineXML(conn->conn, xml);
		if (dom == NULL)
			RETURN_FALSE;
	}

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_change_memory
	Since version:	0.4.2
	Description:	Function is used to change the domain memory allocation
	Arguments:	@res [resource]: libvirt domain resource
			@allocMem [int]: number of MiBs to be set as immediate memory value
			@allocMax [int]: number of MiBs to be set as the maximum allocation
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_change_memory)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmpA = NULL;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	long allocMem = 0;
	long allocMax = 0;
	int retval = -1;
	int pos = -1;
	int len = 0;
	php_libvirt_domain *res_domain = NULL;
	php_libvirt_connection *conn   = NULL;
	virDomainPtr dom = NULL;

	GET_DOMAIN_FROM_ARGS("rll|l",&zdomain,&allocMem, &allocMax, &xflags);

	DPRINTF("%s: Changing domain memory count to %d MiB current/%d MiB max, domain = %p\n",
		PHPFUNC, (int)allocMem, (int)allocMax, domain->domain);

	allocMem *= 1024;
	allocMax *= 1024;

	if (allocMem > allocMax)
		allocMem = allocMax;

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "  <memory>%d</memory>\n  <currentMemory>%d</currentMemory>\n", allocMax, allocMem);
	tmpA = strstr(xml, "<memory>");
	tmp1 = strstr(xml, "</currentMemory>") + strlen("</currentMemory>");
	pos = strlen(xml) - strlen(tmp1);
	len = strlen(xml) - strlen(tmpA);

	tmp2 = emalloc( ( len + 1 )* sizeof(char) );
	memset(tmp2, 0, len + 1);
	memcpy(tmp2, xml, len);

	new_len = strlen(tmp1) + strlen(tmp2) + strlen(new) + 2;
	new_xml = emalloc( new_len * sizeof(char) );
	snprintf(new_xml, new_len, "%s\n%s%s", tmp2, new, tmp1);

	conn = domain->conn;
	
	virDomainUndefine(domain->domain);

	retval = virDomainFree(domain->domain);
	if (retval != 0) {
		DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
	else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
		DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn, new_xml);
	if (dom==NULL) {
		DPRINTF("%s: Function failed, restoring original XML\n", PHPFUNC);
		dom=virDomainDefineXML(conn->conn, xml);
		if (dom == NULL)
			RETURN_FALSE;
	}

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_change_boot_devices
	Since version:	0.4.2
	Description:	Function is used to change the domain boot devices
	Arguments:	@res [resource]: libvirt domain resource
			@first [string]: first boot device to be set
			@second [string]: second boot device to be set
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_change_boot_devices)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmpA = NULL;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	char *first = NULL;
	int first_len;
	char *second = NULL;
	int second_len;
	int retval = -1;
	int pos = -1;
	int len = 0;
	php_libvirt_domain *res_domain = NULL;
	php_libvirt_connection *conn   = NULL;
	virDomainPtr dom = NULL;

	GET_DOMAIN_FROM_ARGS("rss|l",&zdomain,&first, &first_len, &second, &second_len, &xflags);

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	DPRINTF("%s: Changing domain boot order, domain = %p\n", PHPFUNC, domain->domain);

	if (!second || (strcmp(second, "-") == 0))
		snprintf(new, sizeof(new), "    <boot dev='%s'/>\n", first);
	else
		snprintf(new, sizeof(new), "    <boot dev='%s'/>\n    <boot dev='%s'/>\n", first, second);

	tmpA = strstr(xml, "</type>") + strlen("</type>");
	tmp1 = strstr(xml, "</os>");
	pos = strlen(xml) - strlen(tmp1);
	len = strlen(xml) - strlen(tmpA);

	tmp2 = emalloc( ( len + 1 )* sizeof(char) );
	memset(tmp2, 0, len + 1);
	memcpy(tmp2, xml, len);

	new_len = strlen(tmp1) + strlen(tmp2) + strlen(new) + 2;
	new_xml = emalloc( new_len * sizeof(char) );
	snprintf(new_xml, new_len, "%s\n%s%s", tmp2, new, tmp1);

	conn = domain->conn;
	
	virDomainUndefine(domain->domain);

	retval = virDomainFree(domain->domain);
	if (retval != 0) {
		DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
	else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
		DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn, new_xml);
	if (dom==NULL) {
		DPRINTF("%s: Function failed, restoring original XML\n", PHPFUNC);
		dom=virDomainDefineXML(conn->conn, xml);
		if (dom == NULL)
			RETURN_FALSE;
	}

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_disk_add
	Since version:	0.4.2
	Description:	Function is used to add the disk to the virtual machine using set of API functions to make it as simple as possible for the user
	Arguments:	@res [resource]: libvirt domain resource
			@img [string]: string for the image file on the host system
			@dev [string]: string for the device to be presented to the guest (e.g. hda)
			@typ [string]: bus type for the device in the guest, usually 'ide' or 'scsi'
                        @driver [string]: driver type to be specified, like 'raw' or 'qcow2'
			@flags [int]: flags for getting the XML description
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_disk_add)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *img = NULL;
	int img_len;
	char *dev = NULL;
	int dev_len;
	char *driver = NULL;
	int driver_len;
	char *typ = NULL;
	int typ_len;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	int retval = -1;
	int pos = -1;
	php_libvirt_domain *res_domain = NULL;
	php_libvirt_connection *conn   = NULL;
	virDomainPtr dom=NULL;

	GET_DOMAIN_FROM_ARGS("rssss|l",&zdomain,&img,&img_len,&dev,&dev_len,&typ,&typ_len,&driver,&driver_len,&xflags);

	DPRINTF("%s: Domain %p, device = %s, image = %s, type = %s, driver = %s\n", PHPFUNC,
		domain->domain, dev, img, typ, driver);

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "//domain/devices/disk/source[@file=\"%s\"]/./@file", img);
	tmp1 = get_string_from_xpath(xml, new, NULL, &retval);
	if (tmp1 != NULL) {
		free(tmp1);
		snprintf(new, sizeof(new), "Domain already has image <i>%s</i> connected", img);
		set_error(new);
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "//domain/devices/disk/target[@dev='%s']/./@dev", dev);
	tmp1 = get_string_from_xpath(xml, new, NULL, &retval);
	if (tmp1 != NULL) {
		free(tmp1);
		snprintf(new, sizeof(new), "Domain already has device <i>%s</i> connected", dev);
		set_error(new);
		RETURN_FALSE;
	}

	if (access(img, R_OK) != 0) {
		snprintf(new, sizeof(new), "Image file <i>%s</i> doesn't exist", img);
		set_error(new);
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), 
	"    <disk type='file' device='disk'>\n"
	"      <driver name='qemu' type='%s'/>\n"
	"      <source file='%s'/>\n"
	"      <target dev='%s' bus='%s'/>\n"
	"    </disk>", driver, img, dev, typ);
	tmp1 = strstr(xml, "</emulator>") + strlen("</emulator>");
	pos = strlen(xml) - strlen(tmp1);

	tmp2 = emalloc( ( pos + 1 )* sizeof(char) );
	memset(tmp2, 0, pos + 1);
	memcpy(tmp2, xml, pos);

	new_len = strlen(tmp1) + strlen(tmp2) + strlen(new) + 2;
	new_xml = emalloc( new_len * sizeof(char) );
	snprintf(new_xml, new_len, "%s\n%s%s", tmp2, new, tmp1);

	conn = domain->conn;
	
	virDomainUndefine(domain->domain);
	virDomainFree(domain->domain);

	retval = virDomainFree(domain->domain);
	if (retval != 0) {
		DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
	else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
		DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn, new_xml);
	if (dom==NULL) {
		DPRINTF("%s: Function failed, restoring original XML\n", PHPFUNC);
		dom=virDomainDefineXML(conn->conn, xml);
		if (dom == NULL)
			RETURN_FALSE;
	}

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_disk_remove
	Since version:	0.4.2
	Description:	Function is used to remove the disk from the virtual machine using set of API functions to make it as simple as possible
	Arguments:	@res [resource]: libvirt domain resource
			@dev [string]: string for the device to be removed from the guest (e.g. 'hdb')
			@flags [int]: flags for getting the XML description
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_disk_remove)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *dev = NULL;
	int dev_len;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	int retval = -1;
	int pos = -1;
	int i, idx = 0;
	php_libvirt_domain *res_domain=NULL;
	php_libvirt_connection *conn = NULL;
	virDomainPtr dom = NULL;

	GET_DOMAIN_FROM_ARGS("rs|l",&zdomain,&dev,&dev_len,&xflags);

	DPRINTF("%s: Trying to remove %s from domain %p\n", PHPFUNC, dev, domain->domain);

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "//domain/devices/disk/target[@dev='%s']/./@dev", dev);
	tmp1 = get_string_from_xpath(xml, new, NULL, &retval);
	if (tmp1 == NULL) {
		snprintf(new, sizeof(new), "Device <i>%s</i> is not connected to the guest", dev);
		set_error(new);
		RETURN_FALSE;
	}

	free(tmp1);
	
	snprintf(new, sizeof(new), "<target dev='%s'", dev);
	tmp1 = strstr(xml, new) + strlen(new);
	pos = strlen(xml) - strlen(tmp1);

	tmp2 = emalloc( ( pos + 1 )* sizeof(char) );
	memset(tmp2, 0, pos + 1);
	memcpy(tmp2, xml, pos);
	
	for (i = strlen(tmp2) - 5; i > 0; i--)
		if ((tmp2[i] == '<') && (tmp2[i+1] == 'd')
			&& (tmp2[i+2] == 'i') && (tmp2[i+3] == 's')
			&& (tmp2[i+4] == 'k')) {
					tmp2[i-5] = 0;
					break;
				}

	for (i = 0; i < strlen(tmp1) - 7; i++)
		if ((tmp1[i] == '<') && (tmp1[i+1] == '/')
			&& (tmp1[i+2] == 'd') && (tmp1[i+3] == 'i')
			&& (tmp1[i+4] == 's') && (tmp1[i+5] == 'k')
			&& (tmp1[i+6] == '>')) {
					idx = i + 6;
					break;
				}

	new_len = strlen(tmp2) + (strlen(tmp1) - idx);
	new_xml = emalloc( new_len * sizeof(char) );
	memset(new_xml, 0, new_len);
	strcpy(new_xml, tmp2);
	for (i = idx; i < strlen(tmp1) - 1; i++)
		new_xml[ strlen(tmp2) + i - idx ] = tmp1[i];
			
	conn = domain->conn;
	virDomainUndefine(domain->domain);

	retval = virDomainFree(domain->domain);
	if (retval != 0) {
		DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
	else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
		DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn,new_xml);
	if (dom==NULL) RETURN_FALSE;

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_nic_add
	Since version:	0.4.2
	Description:	Function is used to add the NIC card to the virtual machine using set of API functions to make it as simple as possible for the user
	Arguments:	@res [resource]: libvirt domain resource
			@mac [string]: MAC string interpretation to be used for the NIC device
			@network [string]: network name where to connect this NIC
			@model [string]: string of the NIC model
			@flags [int]: flags for getting the XML description
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_nic_add)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *mac = NULL;
	int mac_len;
	char *net = NULL;
	int net_len;
	char *model = NULL;
	int model_len;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	int retval = -1;
	int pos = -1;
	php_libvirt_domain *res_domain = NULL;
	php_libvirt_connection *conn   = NULL;
	virDomainPtr dom=NULL;
	long slot = -1;

	GET_DOMAIN_FROM_ARGS("rsss|l",&zdomain,&mac,&mac_len,&net,&net_len,&model,&model_len,&xflags);
	if (model_len < 1)
		model = NULL;

	DPRINTF("%s: domain = %p, mac = %s, net = %s, model = %s\n", PHPFUNC, domain->domain, mac, net, model);

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "//domain/devices/interface[@type='network']/mac[@address='%s']/./@mac", mac);
	tmp1 = get_string_from_xpath(xml, new, NULL, &retval);
	if (tmp1 != NULL) {
		free(tmp1);
		snprintf(new, sizeof(new), "Domain already has NIC device with MAC address <i>%s</i> connected", mac);
		set_error(new);
		RETURN_FALSE;
	}

	slot = get_next_free_numeric_value(domain->domain, "//@slot");
	if (slot < 0) {
		free(tmp1);
		snprintf(new, sizeof(new), "Cannot find a free slot for domain");
		set_error(new);		
		RETURN_FALSE;
	}
	
	if (model == NULL)
		snprintf(new, sizeof(new), 
		"	<interface type='network'>\n"
		"		<mac address='%s' />\n"
		"		<source network='%s' />\n"
		"		<address type='pci' domain='0x0000' bus='0x00' slot='0x%02x' function='0x0' />\n"
		"	</interface>", mac, net, slot);
	else
		snprintf(new, sizeof(new), 
		"	<interface type='network'>\n"
		"		<mac address='%s' />\n"
		"		<source network='%s' />\n"
		"		<model type='%s' />\n"
		"		<address type='pci' domain='0x0000' bus='0x00' slot='0x%02x' function='0x0' />\n"
		"	</interface>", mac, net, model, slot);
		
	tmp1 = strstr(xml, "</controller>") + strlen("</controller>");
	pos = strlen(xml) - strlen(tmp1);

	tmp2 = emalloc( ( pos + 1 )* sizeof(char) );
	memset(tmp2, 0, pos + 1);
	memcpy(tmp2, xml, pos);

	new_len = strlen(tmp1) + strlen(tmp2) + strlen(new) + 2;
	new_xml = emalloc( new_len * sizeof(char) );
	snprintf(new_xml, new_len, "%s\n%s%s", tmp2, new, tmp1);

	conn = domain->conn;
	
	virDomainUndefine(domain->domain);

	retval = virDomainFree(domain->domain);
	if (retval != 0) {
		DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
	else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
		DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn, new_xml);
	if (dom==NULL) {
		DPRINTF("%s: Function failed, restoring original XML\n", PHPFUNC);
		dom=virDomainDefineXML(conn->conn, xml);
		if (dom == NULL)
			RETURN_FALSE;
	}

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_nic_remove
	Since version:	0.4.2
	Description:	Function is used to remove the NIC from the virtual machine using set of API functions to make it as simple as possible
	Arguments:	@res [resource]: libvirt domain resource
			@dev [string]: string representation of the IP address to be removed (e.g. 54:52:00:xx:yy:zz)
			@flags [int]: optional flags for getting the XML description
	Returns:	new domain resource
*/
PHP_FUNCTION(libvirt_domain_nic_remove)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	char *tmp1 = NULL;
	char *tmp2 = NULL;
	char *xml;
	char *mac = NULL;
	int mac_len;
	char *new_xml = NULL;
	int new_len;
	char new[4096] = { 0 };
	long xflags = 0;
	int retval = -1;
	int pos = -1;
	int i, idx = 0;
	php_libvirt_domain *res_domain=NULL;
	php_libvirt_connection *conn = NULL;
	virDomainPtr dom = NULL;

	GET_DOMAIN_FROM_ARGS("rs|l",&zdomain,&mac,&mac_len,&xflags);

	DPRINTF("%s: Trying to remove NIC device with MAC address %s from domain %p\n", PHPFUNC, mac, domain->domain);

	xml=virDomainGetXMLDesc(domain->domain,xflags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	snprintf(new, sizeof(new), "//domain/devices/interface[@type='network']/mac[@address='%s']/./@address", mac);
	tmp1 = get_string_from_xpath(xml, new, NULL, &retval);
	if (tmp1 == NULL) {
		snprintf(new, sizeof(new), "Network card with IP address <i>%s</i> is not connected to the guest", mac);
		set_error(new);
		RETURN_FALSE;
	}

	free(tmp1);
	
	snprintf(new, sizeof(new), "<mac address='%s'", mac);
	if (strstr(xml, new) == NULL)
		snprintf(new, sizeof(new), "<mac address=\"%s\"", mac);
		
	tmp1 = strstr(xml, new) + strlen(new);
	pos = strlen(xml) - strlen(tmp1);

	tmp2 = emalloc( ( pos + 1 )* sizeof(char) );
	memset(tmp2, 0, pos + 1);
	memcpy(tmp2, xml, pos);
	
	for (i = strlen(tmp2) - 5; i > 0; i--)
		if ((tmp2[i] == '<') && (tmp2[i+1] == 'i')
			&& (tmp2[i+2] == 'n') && (tmp2[i+3] == 't')
			&& (tmp2[i+4] == 'e')) {
					tmp2[i-5] = 0;
					break;
				}

	for (i = 0; i < strlen(tmp1) - 7; i++)
		if ((tmp1[i] == '<') && (tmp1[i+1] == '/')
			&& (tmp1[i+2] == 'i') && (tmp1[i+3] == 'n')
			&& (tmp1[i+4] == 't') && (tmp1[i+5] == 'e')
			&& (tmp1[i+6] == 'r')) {
					idx = i + 6;
					break;
				}

	new_len = strlen(tmp2) + (strlen(tmp1) - idx);
	new_xml = emalloc( new_len * sizeof(char) );
	memset(new_xml, 0, new_len);
	strcpy(new_xml, tmp2);
	for (i = idx; i < strlen(tmp1) - 1; i++)
		new_xml[ strlen(tmp2) + i - idx ] = tmp1[i];
			
	conn = domain->conn;
	virDomainUndefine(domain->domain);

	retval = virDomainFree(domain->domain);
	if (retval != 0) {
                DPRINTF("%s: Cannot free domain %p, error code = %d (%s)\n", PHPFUNC, domain->domain, retval, LIBVIRT_G(last_error));
	}
        else {
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain->domain, 0);
                DPRINTF("%s: Domain %p freed\n", PHPFUNC, domain->domain);
	}

	dom=virDomainDefineXML(conn->conn,new_xml);
	if (dom==NULL) RETURN_FALSE;

	res_domain = emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = dom;
	res_domain->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_get_info
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's information
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	domain information array
*/
PHP_FUNCTION(libvirt_domain_get_info)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	virDomainInfo domainInfo;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainGetInfo(domain->domain,&domainInfo);
	DPRINTF("%s: virDomainGetInfo(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;

	array_init(return_value);
	add_assoc_long(return_value, "maxMem", domainInfo.maxMem);
	add_assoc_long(return_value, "memory", domainInfo.memory);
	add_assoc_long(return_value, "state", (long)domainInfo.state);
	add_assoc_long(return_value, "nrVirtCpu", domainInfo.nrVirtCpu);
	add_assoc_double(return_value, "cpuUsed", (double)((double)domainInfo.cpuTime/1000000000.0));
}

/*
	Function name:	libvirt_domain_create
	Since version:	0.4.1(-1)
	Description:	Function is used to create the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	result of domain creation (startup)
*/
PHP_FUNCTION(libvirt_domain_create)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainCreate(domain->domain);
	DPRINTF("%s: virDomainCreate(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_destroy
	Since version:	0.4.1(-1)
	Description:	Function is used to destroy the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	result of domain destroy
*/
PHP_FUNCTION(libvirt_domain_destroy)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainDestroy(domain->domain);
	DPRINTF("%s: virDomainDestroy(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_resume
	Since version:	0.4.1(-1)
	Description:	Function is used to resume the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	result of domain resume
*/
PHP_FUNCTION(libvirt_domain_resume)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainResume(domain->domain);
	DPRINTF("%s: virDomainResume(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_core_dump
	Since version:	0.4.1(-2)
	Description:	Function is used to dump core of the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_core_dump)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	int to_len;
	char *to;

	GET_DOMAIN_FROM_ARGS("rs",&zdomain,&to,&to_len);

	retval=virDomainCoreDump(domain->domain, to, 0);
	DPRINTF("%s: virDomainCoreDump(%p, %s, 0) returned %d\n", PHPFUNC, domain->domain, to, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_shutdown
	Since version:	0.4.1(-1)
	Description:	Function is used to shutdown the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_shutdown)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainShutdown(domain->domain);
	DPRINTF("%s: virDomainShutdown(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_suspend
	Since version:	0.4.1(-1)
	Description:	Function is used to suspend the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_suspend)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainSuspend(domain->domain);
	DPRINTF("%s: virDomainSuspend(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_undefine
	Since version:	0.4.1(-1)
	Description:	Function is used to undefine the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_undefine)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainUndefine(domain->domain);
	DPRINTF("%s: virDomainUndefine(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_reboot
	Since version:	0.4.1(-1)
	Description:	Function is used to reboot the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_reboot)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	long flags=0;

	GET_DOMAIN_FROM_ARGS("r|l",&zdomain,&flags);

	retval=virDomainReboot(domain->domain,flags);
	DPRINTF("%s: virDomainReboot(%p) returned %d\n", PHPFUNC, domain->domain, retval);
	if (retval != 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_define_xml
	Since version:	0.4.1(-1)
	Description:	Function is used to define the domain from XML string
	Arguments:	@conn [resource]: libvirt connection resource
			@xml [string]: XML string to define guest from
	Returns:	newly defined domain resource
*/
PHP_FUNCTION(libvirt_domain_define_xml)
{
	php_libvirt_domain *res_domain=NULL;
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	virDomainPtr domain=NULL;
	char *xml;
	int xml_len;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&xml,&xml_len);

	domain=virDomainDefineXML(conn->conn,xml);
	if (domain==NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = domain;

        res_domain->conn=conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_create_xml
	Since version:	0.4.1(-1)
	Description:	Function is used to create the domain identified by it's resource
	Arguments:	@conn [resource]: libvirt connection resource
			@xml [string]: XML string to create guest from
	Returns:	newly started/created domain resource
*/
PHP_FUNCTION(libvirt_domain_create_xml)
{
	php_libvirt_domain *res_domain=NULL;
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	virDomainPtr domain=NULL;
	char *xml;
	int xml_len;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&xml,&xml_len);

	domain=virDomainCreateXML(conn->conn,xml,0);
	DPRINTF("%s: virDomainCreateXML(%p, <xml>, 0) returned %p\n", PHPFUNC, conn->conn, domain);
	if (domain==NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = domain;

	res_domain->conn=conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain);
}

/*
	Function name:	libvirt_domain_memory_peek
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's memory peek value
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	domain memory peek
*/
PHP_FUNCTION(libvirt_domain_memory_peek)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	long flags=0;
	long long start;
	long size;
	char *buff;

	GET_DOMAIN_FROM_ARGS("rlll",&zdomain,&start,&size,&flags);
	buff=emalloc(size);
	retval=virDomainMemoryPeek(domain->domain,start,size,buff,flags);
	if (retval != 0) RETURN_FALSE;
	RETURN_STRINGL(buff,size,0);
}

/*
	Function name:	libvirt_domain_memory_stats
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's memory stats
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	domain memory stats array (same fields as virDomainMemoryStats, please see libvirt documentation)
*/
#if LIBVIR_VERSION_NUMBER>=7005
PHP_FUNCTION(libvirt_domain_memory_stats)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	long flags=0;
	int i;
	struct _virDomainMemoryStat stats[VIR_DOMAIN_MEMORY_STAT_NR];

	GET_DOMAIN_FROM_ARGS("r|l",&zdomain,&flags);

	retval=virDomainMemoryStats(domain->domain,stats,VIR_DOMAIN_MEMORY_STAT_NR,flags);
	DPRINTF("%s: virDomainMemoryStats(%p...) returned %d\n", PHPFUNC, domain->domain, retval);

	if (retval == -1) RETURN_FALSE;
	LONGLONG_INIT
	array_init(return_value);
	for (i=0;i<retval;i++)
	{
		LONGLONG_INDEX(return_value, stats[i].tag,stats[i].val)
	} 
}
#else
PHP_FUNCTION(libvirt_domain_memory_stats)
{
	set_error("Only libvirt 0.7.5 and higher supports getting the job information" TSRMLS_CC);
}
#endif

/*
	Function name:	libvirt_domain_update_device
	Since version:	0.4.1(-1)
	Description:	Function is used to update the domain's devices from the XML string
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@xml [string]: XML string for the update
			@flags [int]: Flags to update the device (VIR_DOMAIN_DEVICE_MODIFY_CURRENT, VIR_DOMAIN_DEVICE_MODIFY_LIVE, VIR_DOMAIN_DEVICE_MODIFY_CONFIG, VIR_DOMAIN_DEVICE_MODIFY_FORCE)
	Returns:	TRUE for success, FALSE on error
*/
#if LIBVIR_VERSION_NUMBER>=8000
PHP_FUNCTION(libvirt_domain_update_device)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;	
	char *xml;
	int xml_len;
	long flags;
	int res;
	 
	GET_DOMAIN_FROM_ARGS("rsl",&zdomain,&xml,&xml_len,&flags);

	res=virDomainUpdateDeviceFlags(domain->domain,xml,flags);
	DPRINTF("%s: virDomainUpdateDeviceFlags(%p) returned %d\n", PHPFUNC, domain->domain, res);
	if (res != 0)
		RETURN_FALSE;

	RETURN_TRUE;
}
#else
PHP_FUNCTION(libvirt_domain_update_device)
{
	set_error("Only libvirt 0.8.0 and higher supports updating the device information" TSRMLS_CC);
}
#endif

/*
	Function name:	libvirt_domain_block_stats
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's block stats
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@path [string]: device path to get statistics about
	Returns:	domain block stats array, fields are rd_req, rd_bytes, wr_req, wr_bytes and errs
*/
PHP_FUNCTION(libvirt_domain_block_stats)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	char *path;
	int path_len;
	 	 	 
	struct _virDomainBlockStats stats;
  
	GET_DOMAIN_FROM_ARGS("rs",&zdomain,&path,&path_len);

	retval=virDomainBlockStats(domain->domain,path,&stats, sizeof stats); 
	DPRINTF("%s: virDomainBlockStats(%p,%s,<stats>,<size>) returned %d\n", PHPFUNC, domain->domain, path, retval);
	if (retval == -1) RETURN_FALSE;
 
	array_init(return_value);
	LONGLONG_INIT
	LONGLONG_ASSOC(return_value, "rd_req", stats.rd_req);
	LONGLONG_ASSOC(return_value, "rd_bytes", stats.rd_bytes);
	LONGLONG_ASSOC(return_value, "wr_req", stats.wr_req);
	LONGLONG_ASSOC(return_value, "wr_bytes", stats.wr_bytes);
	LONGLONG_ASSOC(return_value, "errs", stats.errs);
}

/*
	Function name:	libvirt_domain_get_network_info
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's network information
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@mac [string]: mac address of the network device
	Returns:	domain network info array of MAC address, network name and type of NIC card
*/
PHP_FUNCTION(libvirt_domain_get_network_info) {
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	char *mac;
	char *xml;
	char *tmp = NULL;
	int mac_len;
	char fnpath[1024] = { 0 };

	GET_DOMAIN_FROM_ARGS("rs",&zdomain,&mac,&mac_len);

	/* Get XML for the domain */
	xml=virDomainGetXMLDesc(domain->domain, VIR_DOMAIN_XML_INACTIVE);
	if (xml==NULL) {
                set_error("Cannot get domain XML" TSRMLS_CC);
		RETURN_FALSE;
	}

	DPRINTF("%s: Getting network information for NIC with MAC address '%s'\n", PHPFUNC, mac);
	snprintf(fnpath, sizeof(fnpath), "//domain/devices/interface[@type='network']/mac[@address='%s']/../source/@network", mac);
	tmp = get_string_from_xpath(xml, fnpath, NULL, &retval);
	if (tmp == NULL) {
                set_error("Invalid XPath node for source network" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (retval < 0) {
		set_error("Cannot get XPath expression result for network source" TSRMLS_CC);
		RETURN_FALSE;
	}

	array_init(return_value);
	add_assoc_string_ex(return_value, "mac", 4, mac, 1);
	add_assoc_string_ex(return_value, "network", 8, tmp, 1);

	snprintf(fnpath, sizeof(fnpath), "//domain/devices/interface[@type='network']/mac[@address='%s']/../model/@type", mac);
	tmp = get_string_from_xpath(xml, fnpath, NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "nic_type", 9, tmp, 1);
	else
		add_assoc_string_ex(return_value, "nic_type", 9, "default", 1);
}

/*
	Function name:	libvirt_domain_get_block_info
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's block device information
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@dev [string]: device to get block information about
	Returns: domain block device information array of device, file or partition, capacity, allocation and physical size
*/
#if LIBVIR_VERSION_NUMBER>=8000
PHP_FUNCTION(libvirt_domain_get_block_info) {
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	char *dev;
	char *xml;
	char *tmp = NULL;
	int dev_len, isFile;
	char fnpath[1024] = { 0 };

	struct _virDomainBlockInfo info;

	GET_DOMAIN_FROM_ARGS("rs",&zdomain,&dev,&dev_len);

	/* Get XML for the domain */
	xml=virDomainGetXMLDesc(domain->domain, VIR_DOMAIN_XML_INACTIVE);
	if (xml==NULL) {
		set_error("Cannot get domain XML" TSRMLS_CC);
		RETURN_FALSE;
	}

	isFile = 0;
	snprintf(fnpath, sizeof(fnpath), "//domain/devices/disk/target[@dev='%s']/../source/@dev", dev);
	tmp = get_string_from_xpath(xml, fnpath, NULL, &retval);

	if (retval < 0) {
		set_error("Cannot get XPath expression result for device storage" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (retval == 0) {
		snprintf(fnpath, sizeof(fnpath), "//domain/devices/disk/target[@dev='%s']/../source/@file", dev);
		tmp = get_string_from_xpath(xml, fnpath, NULL, &retval);
		if (retval < 0) {
			set_error("Cannot get XPath expression result for file storage" TSRMLS_CC);
			RETURN_FALSE;
		}
		isFile = 1;
	}

	if (retval == 0) {
		set_error("No relevant node found" TSRMLS_CC);
		RETURN_FALSE;
	}

	retval=virDomainGetBlockInfo(domain->domain, tmp, &info,0);

	if (retval == -1) {
		set_error("Cannot get domain block information" TSRMLS_CC);
		RETURN_FALSE;
	}

	array_init(return_value);
	LONGLONG_INIT
	add_assoc_string_ex(return_value, "device", 7, dev, 1);

	if (isFile)
		add_assoc_string_ex(return_value, "file", 5, tmp, 1);
	else
		add_assoc_string_ex(return_value, "partition", 10, tmp, 1);

	snprintf(fnpath, sizeof(fnpath), "//domain/devices/disk/target[@dev='%s']/../driver/@type", dev);
	tmp = get_string_from_xpath(xml, fnpath, NULL, &retval);
	if (tmp != NULL)
		add_assoc_string_ex(return_value, "type", 5, tmp, 1);

	LONGLONG_ASSOC(return_value, "capacity", info.capacity);
	LONGLONG_ASSOC(return_value, "allocation", info.allocation);
	LONGLONG_ASSOC(return_value, "physical", info.physical);
}
#else
PHP_FUNCTION(libvirt_domain_get_block_info)
{
	set_error("Only libvirt 0.8.0 and higher supports getting the block information" TSRMLS_CC);
	RETURN_FALSE;
}
#endif

/*
	Function name:	libvirt_domain_xml_xpath
	Since version:	0.4.1(-1)
	Description:	Function is used to get the result of xPath expression that's run against the domain
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@xpath [string]: xPath expression to parse against the domain
	Returns:	result of the expression in an array
*/
PHP_FUNCTION(libvirt_domain_xml_xpath) {
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	zval *zpath;
	char *xml;
	char *tmp = NULL;
	long path_len=-1, flags = 0;
	int rc = 0;

	GET_DOMAIN_FROM_ARGS("rs|l",&zdomain, &zpath, &path_len, &flags);

	xml=virDomainGetXMLDesc(domain->domain, flags);
	if (xml==NULL) RETURN_FALSE;

	array_init(return_value);

	if ((tmp = get_string_from_xpath(xml, (char *)zpath, &return_value, &rc)) == NULL) {
		free(xml);
		RETURN_FALSE;
	}

	free(tmp);
	free(xml);

	if (rc == 0)
		RETURN_FALSE;

	add_assoc_string_ex(return_value, "xpath", 6, (char *)zpath, 1);
	if (rc < 0)
		add_assoc_long(return_value, "error_code", (long)rc);
}

/*
	Function name:	libvirt_domain_interface_stats
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's interface stats
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@path [string]: path to interface device
	Returns:	interface stats array of {tx|rx}_{bytes|packets|errs|drop} fields
*/
PHP_FUNCTION(libvirt_domain_interface_stats)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	char *path;
	int path_len;

	struct _virDomainInterfaceStats stats;
  
	GET_DOMAIN_FROM_ARGS("rs",&zdomain,&path,&path_len);

	retval=virDomainInterfaceStats(domain->domain,path,&stats, sizeof stats);
	DPRINTF("%s: virDomainInterfaceStats(%p,%s,<stats>,<size>) returned %d\n", PHPFUNC, domain->domain, path, retval);
	if (retval == -1) RETURN_FALSE;
 
	array_init(return_value);
	LONGLONG_INIT
	LONGLONG_ASSOC(return_value, "rx_bytes", stats.rx_bytes);
	LONGLONG_ASSOC(return_value, "rx_packets", stats.rx_packets);
	LONGLONG_ASSOC(return_value, "rx_errs", stats.rx_errs);
	LONGLONG_ASSOC(return_value, "rx_drop", stats.rx_drop);
	LONGLONG_ASSOC(return_value, "tx_bytes", stats.tx_bytes);
	LONGLONG_ASSOC(return_value, "tx_packets", stats.tx_packets);
	LONGLONG_ASSOC(return_value, "tx_errs", stats.tx_errs);
	LONGLONG_ASSOC(return_value, "tx_drop", stats.tx_drop); 
}

/*
	Function name:	libvirt_domain_get_connect
	Since version:	0.4.1(-1)
	Description:	Function is used to get the domain's connection resource. This function should *not* be used!
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	libvirt connection resource
*/
PHP_FUNCTION(libvirt_domain_get_connect)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
        php_libvirt_connection *conn;

	DPRINTF("%s: Warning: libvirt_domain_get_connect() used. This function should not be used!\n", PHPFUNC);

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	conn= domain->conn;
	if (conn->conn == NULL) RETURN_FALSE;
        RETURN_RESOURCE(conn->resource_id);
}

/*
	Function name:	libvirt_domain_migrate_to_uri
	Since version:	0.4.1(-1)
	Description:	Function is used migrate domain to another libvirt daemon specified by it's URI
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@dest_uri [string]: destination URI to migrate to
			@flags [int]: migration flags
			@dname [string]: domain name to rename domain to on destination side
			@bandwidth [int]: migration bandwidth in Mbps
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_migrate_to_uri)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
	long flags=0;
	char *duri;
	int duri_len;
	char *dname;
	int dname_len;
	long bandwidth;	 
 
	dname=NULL;
	dname_len=0;
	bandwidth=0;
	GET_DOMAIN_FROM_ARGS("rsl|sl",&zdomain,&duri,&duri_len,&flags,&dname,&dname_len,&bandwidth);

	retval=virDomainMigrateToURI(domain->domain,duri,flags,dname,bandwidth);
	DPRINTF("%s: virDomainMigrateToURI() returned %d\n", PHPFUNC, retval);

	if (retval == 0) RETURN_TRUE;
	RETURN_FALSE;
}


/*
	Function name:	libvirt_domain_migrate
	Since version:	0.4.1(-1)
	Description:	Function is used migrate domain to another domain
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
			@dest_conn [string]: destination host connection object
			@flags [int]: migration flags
			@dname [string]: domain name to rename domain to on destination side
			@bandwidth [int]: migration bandwidth in Mbps
	Returns:	libvirt domain resource for migrated domain
*/
PHP_FUNCTION(libvirt_domain_migrate)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	php_libvirt_connection *dconn=NULL;
	zval *zdconn;
	virDomainPtr destdomain=NULL;
	php_libvirt_domain *res_domain;

	long flags=0;
	char *dname;
	int dname_len;
	long bandwidth;
	char *uri;
	int uri_len;	 

	dname=NULL;
	dname_len=0;
	bandwidth=0;
	uri_len=0;
	uri=NULL;
	
	GET_DOMAIN_FROM_ARGS("rrl|sl",&zdomain,&zdconn,&flags,&dname,&dname_len,&uri,&uri_len,&bandwidth);
	ZEND_FETCH_RESOURCE(dconn, php_libvirt_connection*, &zdconn, -1, PHP_LIBVIRT_CONNECTION_RES_NAME, le_libvirt_connection);
	if ((dconn==NULL) || (dconn->conn==NULL)) RETURN_FALSE;
 
	destdomain=virDomainMigrate(domain->domain,dconn->conn,flags,dname,uri,bandwidth);
	if (destdomain == NULL) RETURN_FALSE;

	res_domain= emalloc(sizeof(php_libvirt_domain));
	res_domain->domain = destdomain;
        res_domain->conn=dconn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_domain->domain);
	resource_change_counter(INT_RESOURCE_DOMAIN, dconn->conn, res_domain->domain, 1);
 	ZEND_REGISTER_RESOURCE(return_value, res_domain, le_libvirt_domain); 	 
}

/*
	Function name:	libvirt_domain_get_job_info
	Since version:	0.4.1(-1)
	Description:	Function is used get job information for the domain
	Arguments:	@res [resource]: libvirt domain resource, e.g. from libvirt_domain_get_by_*()
	Returns:	job information array of type, time, data, mem and file fields
*/
#if LIBVIR_VERSION_NUMBER>=7007
PHP_FUNCTION(libvirt_domain_get_job_info)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;
 	 	 	 
	struct _virDomainJobInfo jobinfo;
  
	GET_DOMAIN_FROM_ARGS("r",&zdomain);
 
	retval=virDomainGetJobInfo(domain->domain,&jobinfo); 
	if (retval == -1) RETURN_FALSE;
 
	array_init(return_value);
	LONGLONG_INIT
	add_assoc_long(return_value, "type", jobinfo.type);	 
	LONGLONG_ASSOC(return_value, "time_elapsed", jobinfo.timeElapsed);
	LONGLONG_ASSOC(return_value, "time_remaining", jobinfo.timeRemaining);
	LONGLONG_ASSOC(return_value, "data_total", jobinfo.dataTotal);
	LONGLONG_ASSOC(return_value, "data_processed", jobinfo.dataProcessed);
	LONGLONG_ASSOC(return_value, "data_remaining", jobinfo.dataRemaining);
	LONGLONG_ASSOC(return_value, "mem_total", jobinfo.memTotal);
	LONGLONG_ASSOC(return_value, "mem_processed", jobinfo.memProcessed);
	LONGLONG_ASSOC(return_value, "mem_remaining", jobinfo.memRemaining);
	LONGLONG_ASSOC(return_value, "file_total", jobinfo.fileTotal);
	LONGLONG_ASSOC(return_value, "file_processed", jobinfo.fileProcessed);
	LONGLONG_ASSOC(return_value, "file_remaining", jobinfo.fileRemaining); 
}
#else
PHP_FUNCTION(libvirt_domain_get_job_info)
{
	set_error("Only libvirt 0.7.7 and higher supports getting the job information" TSRMLS_CC);
	RETURN_FALSE;
}
#endif

#if LIBVIR_VERSION_NUMBER>=8000
/*
	Function name:	libvirt_domain_has_current_snapshot
	Since version:	0.4.1(-2)
	Description:	Function is used to get the information whether domain has the current snapshot
	Arguments:	@res [resource]: libvirt domain resource
	Returns:	TRUE is domain has the current snapshot, otherwise FALSE (you may need to check for error using libvirt_get_last_error())
*/
PHP_FUNCTION(libvirt_domain_has_current_snapshot)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int retval;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	retval=virDomainHasCurrentSnapshot(domain->domain, 0); 
	if (retval <= 0) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_snapshot_lookup_by_name
	Since version:	0.4.1(-2)
	Description:	This functions is used to lookup for the snapshot by it's name
	Arguments:	@res [resource]: libvirt domain resource
			@name [string]: name of the snapshot to get the resource
	Returns:	domain snapshot resource
*/
PHP_FUNCTION(libvirt_domain_snapshot_lookup_by_name)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int name_len;
	char *name=NULL;
	php_libvirt_snapshot *res_snapshot;
	virDomainSnapshotPtr snapshot = NULL;

	GET_DOMAIN_FROM_ARGS("rs",&zdomain,&name,&name_len);

	if ( (name == NULL) || (name_len<1)) RETURN_FALSE;
	snapshot=virDomainSnapshotLookupByName(domain->domain, name, 0);
	if (snapshot==NULL) RETURN_FALSE;

	res_snapshot = emalloc(sizeof(php_libvirt_snapshot));
	res_snapshot->domain = domain;
	res_snapshot->snapshot = snapshot;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_snapshot->snapshot);
	resource_change_counter(INT_RESOURCE_SNAPSHOT, domain->conn->conn, res_snapshot->snapshot, 1);
 	ZEND_REGISTER_RESOURCE(return_value, res_snapshot, le_libvirt_snapshot);
}

/*
	Function name:	libvirt_domain_snapshot_create
	Since version:	0.4.1(-2)
	Description:	This function creates the domain snapshot for the domain identified by it's resource
	Arguments:	@res [resource]: libvirt domain resource
	Returns:	domain snapshot resource
*/
PHP_FUNCTION(libvirt_domain_snapshot_create)
{
	php_libvirt_domain *domain=NULL;
	php_libvirt_snapshot *res_snapshot;
	zval *zdomain;
	virDomainSnapshotPtr snapshot = NULL;
	
	GET_DOMAIN_FROM_ARGS("r",&zdomain);
 
	snapshot=virDomainSnapshotCreateXML(domain->domain, "<domainsnapshot/>", 0); 
	DPRINTF("%s: virDomainSnapshotCreateXML(%p, <xml>) returned %p\n", PHPFUNC, domain->domain, snapshot);
	if (snapshot == NULL) RETURN_FALSE;

	res_snapshot = emalloc(sizeof(php_libvirt_snapshot));
	res_snapshot->domain = domain;
	res_snapshot->snapshot = snapshot;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_snapshot->snapshot);
	resource_change_counter(INT_RESOURCE_SNAPSHOT, domain->conn->conn, res_snapshot->snapshot, 1);
 	ZEND_REGISTER_RESOURCE(return_value, res_snapshot, le_libvirt_snapshot); 	 
}

/*
	Function name:	libvirt_domain_snapshot_get_xml
	Since version:	0.4.1(-2)
	Description:	Function is used to get the XML description of the snapshot identified by it's resource
	Arguments:	@res [resource]: libvirt snapshot resource
	Returns:	XML description string for the snapshot
*/
PHP_FUNCTION(libvirt_domain_snapshot_get_xml)
{
	char *xml;
	char *xml_out;
	zval *zsnapshot;
	php_libvirt_snapshot *snapshot;
	
	GET_SNAPSHOT_FROM_ARGS("r",&zsnapshot);

	xml = virDomainSnapshotGetXMLDesc(snapshot->snapshot, 0);
	if (xml==NULL) RETURN_FALSE;

	RECREATE_STRING_WITH_E(xml_out,xml);

	RETURN_STRING(xml_out,0);
}

/*
	Function name:	libvirt_domain_snapshot_revert
	Since version:	0.4.1(-2)
	Description:	Function is used to revert the domain state to the state identified by the snapshot
	Arguments:	@res [resource]: libvirt snapshot resource
	Returns:	TRUE on success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_snapshot_revert)
{
	zval *zsnapshot;
	php_libvirt_snapshot *snapshot;
	int ret;
	
	GET_SNAPSHOT_FROM_ARGS("r",&zsnapshot);

	ret = virDomainRevertToSnapshot(snapshot->snapshot, 0);
	DPRINTF("%s: virDomainRevertToSnapshot(%p, 0) returned %d\n", PHPFUNC, snapshot->snapshot, ret);
	if (ret == -1) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_domain_snapshot_delete
	Since version:	0.4.1(-2)
	Description:	Function is used to revert the domain state to the state identified by the snapshot
	Arguments:	@res [resource]: libvirt snapshot resource
			@flags [int]: 0 to delete just snapshot, VIR_SNAPSHOT_DELETE_CHILDREN to delete snapshot children as well
	Returns:	TRUE on success, FALSE on error
*/
PHP_FUNCTION(libvirt_domain_snapshot_delete)
{
	zval *zsnapshot;
	php_libvirt_snapshot *snapshot;
	int flags = 0;
	int retval;
	
	GET_SNAPSHOT_FROM_ARGS("r|l",&zsnapshot, &flags);

	retval = virDomainSnapshotDelete(snapshot->snapshot, flags);
	DPRINTF("%s: virDomainSnapshotDelete(%p, %d) returned %d\n", PHPFUNC, snapshot->snapshot, flags, retval);
	if (retval == -1) RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_list_domain_snapshots
	Since version:	0.4.1(-2)
	Description:	Function is used to list domain snapshots for the domain specified by it's resource
	Arguments:	@res [resource]: libvirt domain resource
	Returns:	libvirt domain snapshot names array
*/
PHP_FUNCTION(libvirt_list_domain_snapshots)
{
	php_libvirt_domain *domain=NULL;
	zval *zdomain;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i;

	GET_DOMAIN_FROM_ARGS("r",&zdomain);

	expectedcount=virDomainSnapshotNum(domain->domain, 0);
	DPRINTF("%s: virDomainSnapshotNum(%p, 0) returned %d\n", PHPFUNC, domain->domain, expectedcount);

	names=emalloc( expectedcount * sizeof(char *) );
	count=virDomainSnapshotListNames(domain->domain, names, expectedcount, 0);
	if ((count != expectedcount) || (count<0)) RETURN_FALSE;
	array_init(return_value);
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value, names[i], 1);
		free(names[i]);
	}
	efree(names);
}
#else
PHP_FUNCTION(libvirt_domain_has_current_snapshot)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}

PHP_FUNCTION(libvirt_domain_snapshot_create)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}

PHP_FUNCTION(libvirt_domain_snapshot_get_xml)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}

PHP_FUNCTION(libvirt_domain_snapshot_lookup_by_name)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}

PHP_FUNCTION(libvirt_domain_snapshot_revert)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}

PHP_FUNCTION(libvirt_domain_snapshot_delete)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}

PHP_FUNCTION(libvirt_list_domain_snapshots)
{
	set_error("Only libvirt 0.8.0 and higher support snapshots" TSRMLS_CC);
	RETURN_FALSE;
}
#endif

/* Storagepool functions */

/*
	Function name:	libvirt_storagepool_lookup_by_name
	Since version:	0.4.1(-1)
	Description:	Function is used to lookup for storage pool by it's name
	Arguments:	@res [resource]: libvirt connection resource
			@name [string]: storage pool name
	Returns:	libvirt storagepool resource
*/
PHP_FUNCTION(libvirt_storagepool_lookup_by_name)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int name_len;
	char *name=NULL;
	virStoragePoolPtr pool=NULL;
	php_libvirt_storagepool *res_pool;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&name,&name_len);

	if ( (name == NULL) || (name_len<1)) RETURN_FALSE;
	pool=virStoragePoolLookupByName(conn->conn,name);
	DPRINTF("%s: virStoragePoolLookupByName(%p, %s) returned %p\n", PHPFUNC, conn->conn, name, pool);
	if (pool==NULL) RETURN_FALSE;

	res_pool = emalloc(sizeof(php_libvirt_storagepool));
	res_pool->pool = pool;
	res_pool->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_pool->pool);
	resource_change_counter(INT_RESOURCE_STORAGEPOOL, conn->conn, res_pool->pool, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_pool, le_libvirt_storagepool);
}

/* Storagepool functions */

/*
	Function name:	libvirt_storagepool_lookup_by_volume
	Since version:	0.4.1(-1)
	Description:	Function is used to lookup for storage pool by a volume
	Arguments:	@res [volume]: volume resource of storage pool
	Returns:	libvirt storagepool resource
*/
PHP_FUNCTION(libvirt_storagepool_lookup_by_volume)
{
	php_libvirt_volume *volume;
	zval *zvolume;
	virStoragePoolPtr pool=NULL;
	php_libvirt_storagepool *res_pool;

	GET_VOLUME_FROM_ARGS ("r", &zvolume);

	pool = virStoragePoolLookupByVolume (volume->volume);
	DPRINTF("%s: virStoragePoolLookupByVolume(%p) returned %p\n", PHPFUNC, volume->volume, pool);
	if (pool == NULL)
		RETURN_FALSE;

	res_pool = emalloc(sizeof(php_libvirt_storagepool));
	res_pool->pool = pool;
	res_pool->conn = volume->conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_pool->pool);
	resource_change_counter(INT_RESOURCE_STORAGEPOOL, res_pool->conn->conn, res_pool->pool, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_pool, le_libvirt_storagepool);
}

/*
	Function name:	libvirt_storagepool_list_volumes
	Since version:	0.4.1(-1)
	Description:	Function is used to list volumes in the specified storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	list of storage volume names in the storage pool in an array using default keys (indexes)
*/
PHP_FUNCTION(libvirt_storagepool_list_volumes)
{
	php_libvirt_storagepool *pool=NULL;
	zval *zpool;
	char **names=NULL;
	int expectedcount=-1;
	int i;
	int count=-1;

	GET_STORAGEPOOL_FROM_ARGS("r",&zpool);

	expectedcount=virStoragePoolNumOfVolumes (pool->pool);
	DPRINTF("%s: virStoragePoolNumOfVolumes(%p) returned %d\n", PHPFUNC, pool->pool, expectedcount);
	names=emalloc(expectedcount*sizeof(char *));

	count=virStoragePoolListVolumes(pool->pool,names,expectedcount);
	DPRINTF("%s: virStoragePoolListVolumes(%p,%p,%d) returned %d\n", PHPFUNC, pool->pool, names, expectedcount, count);
	array_init(return_value);

	if ((count != expectedcount) || (count<0)) RETURN_FALSE;
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value,  names[i],1);
		free(names[i]);
	}

	efree(names);
}

/*
	Function name:	libvirt_storagepool_get_info
	Since version:	0.4.1(-1)
	Description:	Function is used to get information about the storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	storage pool information array of state, capacity, allocation and available space
*/
PHP_FUNCTION(libvirt_storagepool_get_info)
{
	php_libvirt_storagepool *pool=NULL;
	zval *zpool;
	virStoragePoolInfo poolInfo;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS("r",&zpool);

	retval=virStoragePoolGetInfo(pool->pool,&poolInfo);
	DPRINTF("%s: virStoragePoolGetInfo(%p, <info>) returned %d\n", PHPFUNC, pool->pool, retval);
	if (retval != 0) RETURN_FALSE;

	array_init(return_value);

	// @todo: fix the long long returns
	LONGLONG_INIT
	add_assoc_long(return_value, "state", (long)poolInfo.state);
	LONGLONG_ASSOC(return_value, "capacity", poolInfo.capacity);
	LONGLONG_ASSOC(return_value, "allocation", poolInfo.allocation);
	LONGLONG_ASSOC(return_value, "available", poolInfo.available);
}

/*
	Function name:	libvirt_storagevolume_lookup_by_name
	Since version:	0.4.1(-1)
	Description:	Function is used to lookup for storage volume by it's name
	Arguments:	@res [resource]: libvirt storagepool resource
			@name [string]: name of the storage volume to look for
	Returns:	libvirt storagevolume resource
*/
PHP_FUNCTION(libvirt_storagevolume_lookup_by_name)
{
	php_libvirt_storagepool *pool=NULL;
	php_libvirt_volume *res_volume;
	zval *zpool;
	int name_len;
	char *name=NULL;
	virStorageVolPtr volume=NULL;

	GET_STORAGEPOOL_FROM_ARGS("rs",&zpool,&name,&name_len);
	if ( (name == NULL) || (name_len<1)) RETURN_FALSE;

	volume=virStorageVolLookupByName (pool->pool,name);
	DPRINTF("%s: virStorageVolLookupByName(%p, %s) returned %p\n", PHPFUNC, pool->pool, name, volume);
	if (volume==NULL) RETURN_FALSE;

	res_volume = emalloc(sizeof(php_libvirt_volume));
	res_volume->volume = volume;
	res_volume->conn   = pool->conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_volume->volume);
	resource_change_counter(INT_RESOURCE_VOLUME, pool->conn->conn, res_volume->volume, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_volume, le_libvirt_volume);
}

/*
	Function name:	libvirt_storagevolume_lookup_by_path
	Since version:	0.4.1(-2)
	Description:	Function is used to lookup for storage volume by it's path
	Arguments:	@res [resource]: libvirt connection resource
			@path [string]: path of the storage volume to look for
	Returns:	libvirt storagevolume resource
*/
PHP_FUNCTION(libvirt_storagevolume_lookup_by_path)
{
	php_libvirt_connection *conn=NULL;
	php_libvirt_volume *res_volume;
	zval *zconn;
	int name_len;
	char *name=NULL;
	virStorageVolPtr volume=NULL;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&name,&name_len);
	if ( (name == NULL) || (name_len<1)) RETURN_FALSE;

	volume=virStorageVolLookupByPath (conn->conn,name);
	DPRINTF("%s: virStorageVolLookupByPath(%p, %s) returned %p\n", PHPFUNC, conn->conn, name, volume);
	if (volume==NULL)
	{
		set_error_if_unset("Cannot find storage volume on requested path");
		RETURN_FALSE;
	}

	res_volume = emalloc(sizeof(php_libvirt_volume));
	res_volume->volume = volume;
	res_volume->conn   = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_volume->volume);
	resource_change_counter(INT_RESOURCE_VOLUME, conn->conn, res_volume->volume, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_volume, le_libvirt_volume);
}

/*
	Function name:	libvirt_storagevolume_get_name
	Since version:	0.4.1(-2)
	Description:	Function is used to get the storage volume name
	Arguments:	@res [resource]: libvirt storagevolume resource
	Returns:	storagevolume name
*/
PHP_FUNCTION(libvirt_storagevolume_get_name)
{
	php_libvirt_volume *volume = NULL;
	zval *zvolume;
	const char *retval;

	GET_VOLUME_FROM_ARGS ("r", &zvolume);

	retval = virStorageVolGetName (volume->volume);
	DPRINTF("%s: virStorageVolGetName(%p) returned %s\n", PHPFUNC, volume->volume, retval);
	if (retval == NULL) RETURN_FALSE;

	RETURN_STRING (retval, 1);
}

/*
	Function name:	libvirt_storagevolume_get_path
	Since version:	0.4.1(-2)
	Description:	Function is used to get the  storage volume path
	Arguments:	@res [resource]: libvirt storagevolume resource
	Returns:	storagevolume path
*/
PHP_FUNCTION(libvirt_storagevolume_get_path)
{
	php_libvirt_volume *volume = NULL;
	zval *zvolume;
	char *retval;

	GET_VOLUME_FROM_ARGS ("r", &zvolume);

	retval = virStorageVolGetPath(volume->volume);
	DPRINTF("%s: virStorageVolGetPath(%p) returned %s\n", PHPFUNC, volume->volume, retval);
	if (retval == NULL) RETURN_FALSE;

	RETURN_STRING (retval, 1);
}

/*
	Function name:	libvirt_storagevolume_get_info
	Since version:	0.4.1(-1)
	Description:	Function is used to get the storage volume information
	Arguments:	@res [resource]: libvirt storagevolume resource
	Returns:	storage volume information array of type, allocation and capacity
*/
PHP_FUNCTION(libvirt_storagevolume_get_info)
{
	php_libvirt_volume *volume=NULL;
	zval *zvolume;
	virStorageVolInfo volumeInfo;
	int retval;

	GET_VOLUME_FROM_ARGS("r",&zvolume);

	retval=virStorageVolGetInfo(volume->volume,&volumeInfo);
	DPRINTF("%s: virStorageVolGetInfo(%p, <info>) returned %d\n", PHPFUNC, volume->volume, retval);
	if (retval != 0) RETURN_FALSE;

	array_init(return_value);
	LONGLONG_INIT
	add_assoc_long(return_value, "type", (long)volumeInfo.type);
	LONGLONG_ASSOC(return_value, "capacity", volumeInfo.capacity);
	LONGLONG_ASSOC(return_value, "allocation", volumeInfo.allocation);
}

/*
	Function name:	libvirt_storagevolume_get_xml_desc
	Since version:	0.4.1(-1), changed 0.4.2
	Description:	Function is used to get the storage volume XML description
	Arguments:	@res [resource]: libvirt storagevolume resource
			@xpath [string]: optional xPath expression string to get just this entry, can be NULL
	Returns:	storagevolume XML description or result of xPath expression
*/
PHP_FUNCTION(libvirt_storagevolume_get_xml_desc)
{
	php_libvirt_volume *volume=NULL;
	zval *zvolume;
	char *tmp = NULL;
	char *xml;
	char *xml_out;
	char *xpath = NULL;
	int xpath_len;
	long flags=0;
	int retval = -1;

	GET_VOLUME_FROM_ARGS("rs|l",&zvolume,&xpath,&xpath_len,&flags);
	if (xpath_len < 1)
		xpath = NULL;

	DPRINTF("%s: volume = %p, xpath = %s, flags = %ld\n", PHPFUNC, volume->volume, xpath, flags);

	xml=virStorageVolGetXMLDesc(volume->volume,flags);
	if (xml==NULL) {
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	tmp = get_string_from_xpath(xml, xpath, NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		RECREATE_STRING_WITH_E (xml_out, xml);
	} else {
		RECREATE_STRING_WITH_E(xml_out, tmp);
	}

	RETURN_STRING(xml_out,0);
}

/*
	Function name:	libvirt_storagevolume_create_xml
	Since version:	0.4.1(-1)
	Description:	Function is used to create the new storage pool and return the handle to new storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
			@xml [string]: XML string to create the storage volume in the storage pool
	Returns:	libvirt storagevolume resource
*/
PHP_FUNCTION(libvirt_storagevolume_create_xml)
{
	php_libvirt_volume *res_volume=NULL;
	php_libvirt_storagepool *pool=NULL;
	zval *zpool;
	virStorageVolPtr volume=NULL;
	char *xml;
	int xml_len;

	GET_STORAGEPOOL_FROM_ARGS("rs",&zpool,&xml,&xml_len);

	volume=virStorageVolCreateXML(pool->pool,xml,0);
	DPRINTF("%s: virStorageVolCreateXML(%p, <xml>, 0) returned %p\n", PHPFUNC, pool->pool, volume);
	if (volume==NULL) RETURN_FALSE;

	res_volume= emalloc(sizeof(php_libvirt_volume));
	res_volume->volume = volume;
	res_volume->conn   = pool->conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_volume->volume);
	resource_change_counter(INT_RESOURCE_VOLUME, pool->conn->conn, res_volume->volume, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_volume, le_libvirt_volume);
}

/*
	Function name:	libvirt_storagevolume_create_xml_from
	Since version:	0.4.1(-2)
	Description:	Function is used to clone the new storage volume into pool from the orignial volume
	Arguments:	@pool [resource]: libvirt storagepool resource
			@xml [string]: XML string to create the storage volume in the storage pool
			@original_volume [resource]: libvirt storagevolume resource
	Returns:	libvirt storagevolume resource
*/
PHP_FUNCTION(libvirt_storagevolume_create_xml_from)
{
	php_libvirt_volume *res_volume=NULL;
	php_libvirt_storagepool *pool=NULL;
	zval *zpool;

	php_libvirt_volume *pl_volume=NULL;
	zval *zvolume;

	virStorageVolPtr volume=NULL;
	char *xml;
	int xml_len;

	if (zend_parse_parameters (ZEND_NUM_ARGS() TSRMLS_CC, "rsr", &zpool, &xml, &xml_len, &zvolume) == FAILURE)
	{
		set_error("Invalid pool resource, XML or volume resouce");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE (pool, php_libvirt_storagepool*, &zpool, -1, PHP_LIBVIRT_STORAGEPOOL_RES_NAME, le_libvirt_storagepool);
	if ((pool==NULL)||(pool->pool==NULL))RETURN_FALSE;
	ZEND_FETCH_RESOURCE (pl_volume, php_libvirt_volume*, &zvolume, -1, PHP_LIBVIRT_VOLUME_RES_NAME, le_libvirt_volume);
	if ((pl_volume==NULL)||(pl_volume->volume==NULL))RETURN_FALSE;
	resource_change_counter(INT_RESOURCE_VOLUME, NULL, pl_volume->volume, 1);

	volume=virStorageVolCreateXMLFrom(pool->pool,xml, pl_volume->volume, 0);
	DPRINTF("%s: virStorageVolCreateXMLFrom(%p, <xml>, %p, 0) returned %p\n", PHPFUNC, pool->pool, pl_volume->volume, volume);
	if (volume==NULL) RETURN_FALSE;

	res_volume= emalloc(sizeof(php_libvirt_volume));
	res_volume->volume = volume;
	res_volume->conn   = pool->conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_volume->volume);
	resource_change_counter(INT_RESOURCE_VOLUME, pool->conn->conn, res_volume->volume, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_volume, le_libvirt_volume);
}

/*
	Function name:	libvirt_storagevolume_delete
	Since version:	0.4.2
	Description:	Function is used to delete to volume identified by it's resource
	Arguments:	@res [resource]: libvirt storagevolume resource
			@flags [int]: optional flags for the storage volume deletion for virStorageVolDelete()
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_storagevolume_delete)
{
	php_libvirt_volume *volume=NULL;
	zval *zvolume;
	int flags = 0;
	int retval = 0;

	GET_VOLUME_FROM_ARGS("r|l",&zvolume,&flags);

	retval = virStorageVolDelete(volume->volume, flags);
	DPRINTF("%s: virStorageVolDelete(%p, %d) returned %d\n", PHPFUNC, volume->volume, flags, retval);
        if (retval != 0) {
		set_error_if_unset("Cannot delete storage volume");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/*
	Function name:	libvirt_storagepool_get_uuid_string
	Since version:	0.4.1(-1)
	Description:	Function is used to get storage pool by UUID string
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	storagepool UUID string
*/
PHP_FUNCTION(libvirt_storagepool_get_uuid_string)
{
	php_libvirt_storagepool *pool=NULL;
	zval *zpool;
	char *uuid;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	uuid = emalloc (VIR_UUID_STRING_BUFLEN);
	retval = virStoragePoolGetUUIDString (pool->pool, uuid);
	DPRINTF("%s: virStoragePoolGetUUIDString(%p, %p) returned %d (%s)\n", PHPFUNC, pool->pool, uuid, retval, uuid);
	if (retval != 0)
		RETURN_FALSE;

	RETURN_STRING(uuid, 0);
}

/*
	Function name:	libvirt_storagepool_get_name
	Since version:	0.4.1(-1)
	Description:	Function is used to get storage pool name from the storage pool resource
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	storagepool name string
*/
PHP_FUNCTION(libvirt_storagepool_get_name)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	const char *name=NULL;

	GET_STORAGEPOOL_FROM_ARGS("r", &zpool);

	name = virStoragePoolGetName (pool->pool);
	DPRINTF("%s: virStoragePoolGetName(%p) returned %s\n", PHPFUNC, pool->pool, name);
	if (name == NULL)
		RETURN_FALSE;

	RETURN_STRING(name, 1);
}

/*
	Function name:	libvirt_storagepool_lookup_by_uuid_string
	Since version:	0.4.1(-1)
	Description:	Function is used to lookup for storage pool identified by UUID string
	Arguments:	@res [resource]: libvirt connection resource
			@uuid [string]: UUID string to look for storagepool
	Returns:	libvirt storagepool resource
*/
PHP_FUNCTION(libvirt_storagepool_lookup_by_uuid_string)
{
	php_libvirt_connection *conn = NULL;
	zval *zconn;
	char *uuid = NULL;
	int uuid_len;
	virStoragePoolPtr storage=NULL;
	php_libvirt_storagepool *res_pool;

	GET_CONNECTION_FROM_ARGS("rs", &zconn, &uuid, &uuid_len);

	if ((uuid == NULL) || (uuid_len < 1))
		RETURN_FALSE;

	storage = virStoragePoolLookupByUUIDString (conn->conn, uuid);
	DPRINTF("%s: virStoragePoolLookupByUUIDString(%p, %s) returned %p\n", PHPFUNC, conn->conn, uuid, storage);
	if (storage == NULL)
		RETURN_FALSE;

	res_pool = emalloc (sizeof (php_libvirt_storagepool));
	res_pool->pool = storage;
	res_pool->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_pool->pool);
	resource_change_counter(INT_RESOURCE_STORAGEPOOL, conn->conn, res_pool->pool, 1);
	ZEND_REGISTER_RESOURCE (return_value, res_pool, le_libvirt_storagepool);
}

/*
	Function name:	libvirt_storagepool_get_xml_desc
	Since version:	0.4.1(-1), changed 0.4.2
	Description:	Function is used to get the XML description for the storage pool identified by res
	Arguments:	@res [resource]: libvirt storagepool resource
			@xpath [string]: optional xPath expression string to get just this entry, can be NULL
	Returns:	storagepool XML description string or result of xPath expression
*/
PHP_FUNCTION(libvirt_storagepool_get_xml_desc)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	char *xml;
	char *xml_out;
	char *xpath = NULL;
	char *tmp = NULL;
	long flags = 0;
	int xpath_len;
	int retval = -1;

	GET_STORAGEPOOL_FROM_ARGS("rs|l", &zpool, &xpath, &xpath_len, &flags);
	if (xpath_len < 1)
		xpath = NULL;

	DPRINTF("%s: pool = %p, flags = %ld, xpath = %s\n", PHPFUNC, pool->pool, flags, xpath);

	xml = virStoragePoolGetXMLDesc (pool->pool, flags);
	if (xml == NULL)
	{
		set_error_if_unset("Cannot get the XML description");
		RETURN_FALSE;
	}

	tmp = get_string_from_xpath(xml, xpath, NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		RECREATE_STRING_WITH_E (xml_out, xml);
	} else {
		RECREATE_STRING_WITH_E (xml_out, tmp);
	}

	RETURN_STRING (xml_out, 0);
}

/*
	Function name:	libvirt_storagepool_define_xml
	Since version:	0.4.1(-1)
	Description:	Function is used to define the storage pool from XML string and return it's resource
	Arguments:	@res [resource]: libvirt connection resource
			@xml [string]: XML string definition of storagepool
			@flags [int]: flags to define XML
	Returns:	libvirt storagepool resource
*/
PHP_FUNCTION(libvirt_storagepool_define_xml)
{
	php_libvirt_storagepool *res_pool = NULL;
	php_libvirt_connection *conn = NULL;
	zval *zconn;
	virStoragePoolPtr pool = NULL;
	char *xml;
	int xml_len;
	long flags = 0;


	GET_CONNECTION_FROM_ARGS ("rs|l", &zconn, &xml, &xml_len, &flags);

	pool = virStoragePoolDefineXML (conn->conn, xml, (unsigned int)flags);
	DPRINTF("%s: virStoragePoolDefineXML(%p, <xml>) returned %p\n", PHPFUNC, conn->conn, pool);
	if (pool == NULL)
		RETURN_FALSE;

	res_pool = emalloc (sizeof (php_libvirt_storagepool));
	res_pool->pool = pool;
	res_pool->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_pool->pool);
	resource_change_counter(INT_RESOURCE_STORAGEPOOL, conn->conn, res_pool->pool, 1);
	ZEND_REGISTER_RESOURCE (return_value, res_pool, le_libvirt_storagepool);
}

/*
	Function name:	libvirt_storagepool_undefine
	Since version:	0.4.1(-1)
	Description:	Function is used to undefine the storage pool identified by it's resource
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	TRUE if success, FALSE on error
*/
PHP_FUNCTION(libvirt_storagepool_undefine)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	int retval = 0;
	
	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	retval = virStoragePoolUndefine(pool->pool);
	DPRINTF("%s: virStoragePoolUndefine(%p) returned %d\n", PHPFUNC, pool->pool, retval);
	if (retval != 0)
		RETURN_FALSE;

	RETURN_TRUE;
}

/*
	Function name:	libvirt_storagepool_create
	Since version:	0.4.1(-1)
	Description:	Function is used to create/start the storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	TRUE if success, FALSE on error
*/
PHP_FUNCTION(libvirt_storagepool_create)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	retval = virStoragePoolCreate (pool->pool, 0);
	DPRINTF("%s: virStoragePoolCreate(%p, 0) returned %d\n", PHPFUNC, pool->pool, retval);
	if (retval != 0)
		RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_storagepool_destroy
	Since version:	0.4.1(-1)
	Description:	Function is used to destory the storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	TRUE if success, FALSE on error
*/
PHP_FUNCTION(libvirt_storagepool_destroy)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	retval = virStoragePoolDestroy(pool->pool);
	DPRINTF("%s: virStoragePoolDestroy(%p) returned %d\n", PHPFUNC, pool->pool, retval);
	if (retval != 0)
		RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_storagepool_is_active
	Since version:	0.4.1(-1)
	Description:	Function is used to get information whether storage pool is active or not
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	result of virStoragePoolIsActive
*/
PHP_FUNCTION(libvirt_storagepool_is_active)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);
	
	RETURN_LONG (virStoragePoolIsActive (pool->pool));
}

/*
	Function name:	libvirt_storagepool_get_volume_count
	Since version:	0.4.1(-1)
	Description:	Function is used to get storage volume count in the storage pool
	Arguments:		@res [resource]: libvirt storagepool resource
	Returns:		number of volumes in the pool
*/
PHP_FUNCTION(libvirt_storagepool_get_volume_count)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	RETURN_LONG (virStoragePoolNumOfVolumes(pool->pool));
}

/*
	Function name:	libvirt_storagepool_refresh
	Since version:	0.4.1(-1)
	Description:	Function is used to refresh the storage pool information
	Arguments:	@res [resource]: libvirt storagepool resource
			@flags [int]: refresh flags
	Returns:	TRUE if success, FALSE on error
*/
PHP_FUNCTION(libvirt_storagepool_refresh)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	unsigned long flags = 0;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS ("r|l", &zpool, &flags);

	retval = virStoragePoolRefresh(pool->pool, flags);
	DPRINTF("%s: virStoragePoolRefresh(%p, %ld) returned %d\n", PHPFUNC, pool->pool, flags, retval);
	if (retval < 0)
		RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_storagepool_set_autostart
	Since version:	0.4.1(-1)
	Description:	Function is used to set autostart of the storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
			@flags [int]: flags to set autostart
	Returns:	result on setting storagepool autostart value
*/
PHP_FUNCTION(libvirt_storagepool_set_autostart)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	zend_bool flags = 0;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS ("rb", &zpool, &flags);

	retval = virStoragePoolSetAutostart(pool->pool, flags);
	DPRINTF("%s: virStoragePoolSetAutostart(%p, %d) returned %d\n", PHPFUNC, pool->pool, flags, retval);
	if (retval != 0)
		RETURN_FALSE;
	RETURN_TRUE;
}

/*
	Function name:	libvirt_storagepool_get_autostart
	Since version:	0.4.1(-1)
	Description:	Function is used to get autostart of the storage pool
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	TRUE for autostart enabled, FALSE for autostart disabled, FALSE with last_error set for error
*/
PHP_FUNCTION(libvirt_storagepool_get_autostart)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	int flags = 0;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	if (virStoragePoolGetAutostart (pool->pool, &flags) == 0)
	{
		if (flags == 0) {
			RETURN_FALSE;
		}
		else {
			RETURN_TRUE;
		}
	}
	else
	{
		RETURN_FALSE;
	}
}

/*
	Function name:	libvirt_storagepool_build
	Since version:	0.4.2
	Description:	Function is used to Build the underlying storage pool, e.g. create the destination directory for NFS
	Arguments:	@res [resource]: libvirt storagepool resource
	Returns:	TRUE if success, FALSE on error
*/
PHP_FUNCTION(libvirt_storagepool_build)
{
	php_libvirt_storagepool *pool = NULL;
	zval *zpool;
	int flags = 0;
	int retval;

	GET_STORAGEPOOL_FROM_ARGS ("r", &zpool);

	retval = virStoragePoolBuild(pool->pool, flags);
	DPRINTF("%s: virStoragePoolBuild(%p, %d) returned %d\n", PHPFUNC, pool->pool, flags, retval);
	if (retval == 0)
		RETURN_TRUE;
	
	RETURN_FALSE;
}

/* Listing functions */
/*
	Function name:	libvirt_list_storagepools
	Since version:	0.4.1(-1)
	Description:	Function is used to list storage pools on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt storagepool names array for the connection
*/
PHP_FUNCTION(libvirt_list_storagepools)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount=virConnectNumOfStoragePools(conn->conn);

	names=emalloc(expectedcount*sizeof(char *));
	count=virConnectListStoragePools(conn->conn,names,expectedcount);

	if ((count != expectedcount) || (count<0))
	{
		efree (names);
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value,  names[i],1);
		free(names[i]);
	}
	efree(names);


	expectedcount = virConnectNumOfDefinedStoragePools (conn->conn);
	names= emalloc (expectedcount * sizeof(char *));
	count = virConnectListDefinedStoragePools (conn->conn, names, expectedcount);
	if ((count != expectedcount) || (count < 0))
	{
		efree (names);
		RETURN_FALSE;
	}

	for (i = 0; i < count; i++)
	{
		add_next_index_string (return_value, names[i], 1);
		free (names[i]);
	}
	efree (names);
}

/*
	Function name:	libvirt_list_active_storagepools
	Since version:	0.4.1(-1)
	Description:	Function is used to list active storage pools on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt storagepool names array for the connection
*/
PHP_FUNCTION(libvirt_list_active_storagepools)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount=virConnectNumOfStoragePools(conn->conn);

	names=emalloc(expectedcount*sizeof(char *));
	count=virConnectListStoragePools(conn->conn,names,expectedcount);

	if ((count != expectedcount) || (count<0))
	{
		efree (names);
		RETURN_FALSE;
	}
	array_init(return_value);
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value,  names[i],1);
		free(names[i]);
	}
	efree(names);
}

/*
	Function name:	libvirt_list_inactive_storagepools
	Since version:	0.4.1(-1)
	Description:	Function is used to list inactive storage pools on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt storagepool names array for the connection
*/
PHP_FUNCTION(libvirt_list_inactive_storagepools)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount = virConnectNumOfDefinedStoragePools (conn->conn);
	names= emalloc (expectedcount * sizeof(char *));
	count = virConnectListDefinedStoragePools (conn->conn, names, expectedcount);
	if ((count != expectedcount) || (count < 0))
	{
		efree (names);
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i = 0; i < count; i++)
	{
		add_next_index_string (return_value, names[i], 1);
		free (names[i]);
	}
	efree (names);
}

/*
	Function name:	libvirt_list_domains
	Since version:	0.4.1(-1)
	Description:	Function is used to list domains on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt domain names array for the connection
*/
PHP_FUNCTION(libvirt_list_domains)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	int *ids;
	char **names;
	const char *name;
	int i, rv;
	virDomainPtr domain=NULL;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount=virConnectNumOfDomains (conn->conn);

	ids=emalloc(sizeof(int)*expectedcount);
	count=virConnectListDomains (conn->conn,ids,expectedcount);

	array_init(return_value);
	for (i=0;i<count;i++)
	{
		domain=virDomainLookupByID(conn->conn,ids[i]);
		resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain, 1);
		if (domain!=NULL) 
		{
			name=virDomainGetName(domain);
			if (name==NULL) RETURN_FALSE;
			add_next_index_string(return_value, name, 1);
		}
		rv = virDomainFree (domain);
		if (rv != 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"virDomainFree failed with %i on list_domain: %s",
					rv, LIBVIRT_G (last_error));
		}
		else {
			resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain, 0);
		}
		domain = NULL;
	}
  	efree(ids);

	expectedcount=virConnectNumOfDefinedDomains (conn->conn);
	names=emalloc(expectedcount*sizeof(char *));
	count=virConnectListDefinedDomains (conn->conn,names	,expectedcount);
	if ((count != expectedcount) || (count<0)) RETURN_FALSE;
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value, names[i], 1);
		free(names[i]);
	}
	efree(names);
}

/*
	Function name:	libvirt_list_domain_resources
	Since version:	0.4.1(-1)
	Description:	Function is used to list domain resources on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt domain resources array for the connection
*/
PHP_FUNCTION(libvirt_list_domain_resources)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	zval *zdomain;
	int count=-1;
	int expectedcount=-1;
	int *ids;
	char **names;
	int i;

	virDomainPtr domain=NULL;
	php_libvirt_domain *res_domain;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount=virConnectNumOfDomains (conn->conn);

	ids=emalloc(sizeof(int)*expectedcount);
	count=virConnectListDomains (conn->conn,ids,expectedcount);
	if ((count != expectedcount) || (count<0))
	{
		efree (ids);
		RETURN_FALSE;
	}
	array_init(return_value);
	for (i=0;i<count;i++)
	{
		domain=virDomainLookupByID(conn->conn,ids[i]);
		if (domain!=NULL) 
		{
			res_domain= emalloc(sizeof(php_libvirt_domain));
			res_domain->domain = domain;

			ALLOC_INIT_ZVAL(zdomain);
			res_domain->conn=conn;

			resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
			ZEND_REGISTER_RESOURCE(zdomain, res_domain, le_libvirt_domain);
			add_next_index_zval(return_value,  zdomain);
		}
	}
  	efree(ids);

	expectedcount=virConnectNumOfDefinedDomains (conn->conn);
	names=emalloc(expectedcount*sizeof(char *));
	count=virConnectListDefinedDomains (conn->conn,names	,expectedcount);
	if ((count != expectedcount) || (count<0))
	{
		efree (names);
		RETURN_FALSE;
	}
	for (i=0;i<count;i++)
	{
		domain=virDomainLookupByName	(conn->conn,names[i]);
		if (domain!=NULL) 
		{
			res_domain= emalloc(sizeof(php_libvirt_domain));
			res_domain->domain = domain;

			ALLOC_INIT_ZVAL(zdomain);
		        res_domain->conn=conn;

			ZEND_REGISTER_RESOURCE(zdomain, res_domain, le_libvirt_domain);
			resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, res_domain->domain, 1);
			add_next_index_zval(return_value,  zdomain);
		}
		free(names[i]);
	}
	efree(names);
}

/*
	Function name:	libvirt_list_active_domain_ids
	Since version:	0.4.1(-1)
	Description:	Function is used to list active domain IDs on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt active domain ids array for the connection
*/
PHP_FUNCTION(libvirt_list_active_domain_ids)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	int *ids;
	int i;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount=virConnectNumOfDomains (conn->conn);

	ids=emalloc(sizeof(int)*expectedcount);
	count=virConnectListDomains (conn->conn,ids,expectedcount);
	if ((count != expectedcount) || (count<0))
	{
		efree (ids);
		RETURN_FALSE;
	}
	array_init(return_value);
	for (i=0;i<count;i++)
	{
		add_next_index_long(return_value,  ids[i]);
	}
	efree(ids);  
}

/*
	Function name:	libvirt_list_active_domains
	Since version:	0.4.1(-1)
	Description:	Function is used to list active domain names on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt active domain names array for the connection
*/
PHP_FUNCTION(libvirt_list_active_domains)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	int *ids;
	int i;
	virDomainPtr domain = NULL;
	const char *name;

	GET_CONNECTION_FROM_ARGS("r",&zconn);

	expectedcount=virConnectNumOfDomains (conn->conn);

	ids=emalloc(sizeof(int)*expectedcount);
	count=virConnectListDomains (conn->conn,ids,expectedcount);
	if ((count != expectedcount) || (count<0))
	{
		efree (ids);
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i=0;i<count;i++)
	{
		domain=virDomainLookupByID(conn->conn,ids[i]);
		if (domain!=NULL) 
		{
			resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain, 1);
			name=virDomainGetName(domain);
			if (name==NULL)
			{
				efree (ids);
				if (virDomainFree (domain))
					resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain, 0);
				RETURN_FALSE;
			}

			add_next_index_string(return_value, name, 1);
			if (virDomainFree (domain))
				resource_change_counter(INT_RESOURCE_DOMAIN, conn->conn, domain, 0);
		}
	}
	efree(ids);
}

/*
	Function name:	libvirt_list_inactive_domains
	Since version:	0.4.1(-1)
	Description:	Function is used to list inactive domain names on the connection
	Arguments:	@res [resource]: libvirt connection resource
	Returns:	libvirt inactive domain names array for the connection
*/
PHP_FUNCTION(libvirt_list_inactive_domains)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i;

	GET_CONNECTION_FROM_ARGS("r",&zconn);
	  
	array_init(return_value);
	expectedcount=virConnectNumOfDefinedDomains (conn->conn);

	names=emalloc(expectedcount*sizeof(char *));
	count=virConnectListDefinedDomains (conn->conn,names	,expectedcount);
	if ((count != expectedcount) || (count<0))
	{
		efree (names);
		RETURN_FALSE;
	}
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value,  names[i],1);
		free(names[i]);
	}
	efree(names);
}

/*
	Function name:	libvirt_list_networks
	Since version:	0.4.1(-1)
	Description:	Function is used to list networks on the connection
	Arguments:	@res [resource]: libvirt connection resource
			@flags [int]: flags whether to list active, inactive or all networks (VIR_NETWORKS_{ACTIVE|INACTIVE|ALL} constants)
	Returns:	libvirt network names array for the connection
*/
PHP_FUNCTION(libvirt_list_networks)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	long flags = VIR_NETWORKS_ACTIVE | VIR_NETWORKS_INACTIVE;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i, done = 0;

	GET_CONNECTION_FROM_ARGS("r|l",&zconn,&flags);

	array_init(return_value);
	if (flags & VIR_NETWORKS_ACTIVE) {
		expectedcount=virConnectNumOfNetworks(conn->conn);
		names=emalloc(expectedcount*sizeof(char *));
		count=virConnectListNetworks(conn->conn,names,expectedcount);
		if ((count != expectedcount) || (count<0))
		{
			efree (names);
			RETURN_FALSE;
		}

		for (i=0;i<count;i++)
		{
			add_next_index_string(return_value,  names[i], 1);
			free(names[i]);
		}

		efree(names);
		done++;
	}

	if (flags & VIR_NETWORKS_INACTIVE) {
		expectedcount=virConnectNumOfDefinedNetworks(conn->conn);
		names=emalloc(expectedcount*sizeof(char *));
		count=virConnectListDefinedNetworks(conn->conn,names,expectedcount);
		if ((count != expectedcount) || (count<0))
		{
			efree (names);
			RETURN_FALSE;
		}

		for (i=0;i<count;i++)
		{
			add_next_index_string(return_value, names[i], 1);
			free(names[i]);
		}

		efree(names);
		done++;
	}

	if (!done)
		RETURN_FALSE;
}

/*
	Function name:	libvirt_list_nodedevs
	Since version:	0.4.1(-1)
	Description:	Function is used to list node devices on the connection
	Arguments:	@res [resource]: libvirt connection resource
			@cap [string]: optional capability string
	Returns:	libvirt nodedev names array for the connection
*/
PHP_FUNCTION(libvirt_list_nodedevs)
{
	php_libvirt_connection *conn=NULL;
	zval *zconn;
	int count=-1;
	int expectedcount=-1;
	char *cap = NULL;
	char **names;
	int i, cap_len;

	GET_CONNECTION_FROM_ARGS("r|s",&zconn,&cap,&cap_len);

	expectedcount=virNodeNumOfDevices(conn->conn, cap, 0);
	names=emalloc(expectedcount*sizeof(char *));
	count=virNodeListDevices(conn->conn, cap, names, expectedcount, 0);
	if ((count != expectedcount) || (count<0))
	{
		efree (names);
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value,  names[i], 1);
		free(names[i]);
	}

	efree(names);
}

/* Nodedev functions */
/*
	Function name:	libvirt_nodedev_get
	Since version:	0.4.1(-1)
	Description:	Function is used to get the node device by it's name
	Arguments:	@res [resource]: libvirt connection resource
			@name [string]: name of the nodedev to get resource
	Returns:	libvirt nodedev resource
*/
PHP_FUNCTION(libvirt_nodedev_get)
{
	php_libvirt_connection *conn = NULL;
	php_libvirt_nodedev *res_dev = NULL;
	virNodeDevice *dev;
	zval *zconn;
	char *name;
	int name_len;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&name,&name_len);

	if ((dev = virNodeDeviceLookupByName(conn->conn, name)) == NULL) {
		set_error("Cannot get find requested node device" TSRMLS_CC);
		RETURN_FALSE;
	}

	res_dev = emalloc(sizeof(php_libvirt_nodedev));
	res_dev->device = dev;
	res_dev->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_dev->device);
	resource_change_counter(INT_RESOURCE_NODEDEV, conn->conn, res_dev->device, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_dev, le_libvirt_nodedev);
}

/*
	Function name:	libvirt_nodedev_capabilities
	Since version:	0.4.1(-1)
	Description:	Function is used to list node devices by capabilities
	Arguments:	@res [resource]: libvirt nodedev resource
	Returns:	nodedev capabilities array
*/
PHP_FUNCTION(libvirt_nodedev_capabilities)
{
	php_libvirt_nodedev *nodedev=NULL;
	zval *znodedev;
	int count=-1;
	int expectedcount=-1;
	char **names;
	int i;

	GET_NODEDEV_FROM_ARGS("r",&znodedev);

	expectedcount=virNodeDeviceNumOfCaps(nodedev->device);
	names=emalloc(expectedcount*sizeof(char *));
	count=virNodeDeviceListCaps(nodedev->device, names, expectedcount);
	if ((count != expectedcount) || (count<0)) RETURN_FALSE;

	array_init(return_value);
	for (i=0;i<count;i++)
	{
		add_next_index_string(return_value, names[i], 1);
		free(names[i]);
	}

	efree(names);
}

/*
	Function name:	libvirt_nodedev_get_xml_desc
	Since version:	0.4.1(-1), changed 0.4.2
	Description:	Function is used to get the node device's XML description
	Arguments:	@res [resource]: libvirt nodedev resource
			@xpath [string]: optional xPath expression string to get just this entry, can be NULL
	Returns:	nodedev XML description string or result of xPath expression
*/
PHP_FUNCTION(libvirt_nodedev_get_xml_desc)
{
	php_libvirt_nodedev *nodedev=NULL;
	zval *znodedev;
	char *tmp = NULL;
	char *xml = NULL;
	char *xml_out = NULL;
	char *xpath = NULL;
	int xpath_len;
	int retval = -1;

	GET_NODEDEV_FROM_ARGS("r|s",&znodedev,&xpath,&xpath_len);
	if (xpath_len < 1)
	{
		xpath = NULL;
	}

	xml=virNodeDeviceGetXMLDesc(nodedev->device, 0);
	if ( xml == NULL ) {
		set_error("Cannot get the device XML information" TSRMLS_CC);
		RETURN_FALSE;
	}

	tmp = get_string_from_xpath(xml, xpath, NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		RECREATE_STRING_WITH_E (xml_out, xml);
	} else {
		RECREATE_STRING_WITH_E (xml_out, tmp);
	}

	RETURN_STRING(xml_out, 0);
}

/*
	Function name:	libvirt_nodedev_get_information
	Since version:	0.4.1(-1)
	Description:	Function is used to get the node device's information
	Arguments:	@res [resource]: libvirt nodedev resource
	Returns:	nodedev information array
*/
PHP_FUNCTION(libvirt_nodedev_get_information)
{
	php_libvirt_nodedev *nodedev=NULL;
	zval *znodedev;
	int retval = -1;
	char *xml = NULL;
	char *tmp = NULL;
	char *cap = NULL;

	GET_NODEDEV_FROM_ARGS("r",&znodedev);

	xml=virNodeDeviceGetXMLDesc(nodedev->device, 0);
	if ( xml == NULL ) {
		set_error("Cannot get the device XML information" TSRMLS_CC);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* Get name */
	tmp = get_string_from_xpath(xml, "//device/name", NULL, &retval);
	if (tmp == NULL) {
		set_error("Invalid XPath node for device name" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (retval < 0) {
		set_error("Cannot get XPath expression result for device name" TSRMLS_CC);
		RETURN_FALSE;
	}

	add_assoc_string_ex(return_value, "name", 5, tmp, 1);

	/* Get parent name */
	tmp = get_string_from_xpath(xml, "//device/parent", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "parent", 7, tmp, 1);

	/* Get capability */
	cap = get_string_from_xpath(xml, "//device/capability/@type", NULL, &retval);
	if ((cap != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "capability", 11, cap, 1);

	/* System capability is having hardware and firmware sub-blocks */
	if (strcmp(cap, "system") == 0) {
		/* Get hardware vendor */
		tmp = get_string_from_xpath(xml, "//device/capability/hardware/vendor", NULL, &retval);
		if ((tmp != NULL) && (retval > 0))
			add_assoc_string_ex(return_value, "hardware_vendor", 16, tmp, 1);

		/* Get hardware version */
		tmp = get_string_from_xpath(xml, "//device/capability/hardware/version", NULL, &retval);
		if ((tmp != NULL) && (retval > 0))
			add_assoc_string_ex(return_value, "hardware_version", 17, tmp, 1);

		/* Get hardware serial */
		tmp = get_string_from_xpath(xml, "//device/capability/hardware/serial", NULL, &retval);
		if ((tmp != NULL) && (retval > 0))
			add_assoc_string_ex(return_value, "hardware_serial", 16, tmp, 1);

		/* Get hardware UUID */
		tmp = get_string_from_xpath(xml, "//device/capability/hardware/uuid", NULL, &retval);
		if (tmp != NULL)
			add_assoc_string_ex(return_value, "hardware_uuid", 15, tmp, 1);

		/* Get firmware vendor */
		tmp = get_string_from_xpath(xml, "//device/capability/firmware/vendor", NULL, &retval);
		if ((tmp != NULL) && (retval > 0))
			add_assoc_string_ex(return_value, "firmware_vendor", 16, tmp, 1);

		/* Get firmware version */
		tmp = get_string_from_xpath(xml, "//device/capability/firmware/version", NULL, &retval);
		if ((tmp != NULL) && (retval > 0))
			add_assoc_string_ex(return_value, "firmware_version", 17, tmp, 1);

		/* Get firmware release date */
		tmp = get_string_from_xpath(xml, "//device/capability/firmware/release_date", NULL, &retval);
		if ((tmp != NULL) && (retval > 0))
			add_assoc_string_ex(return_value, "firmware_release_date", 22, tmp, 1);
	}

	/* Get product_id */
	tmp = get_string_from_xpath(xml, "//device/capability/product/@id", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "product_id", 11, tmp, 1);

	/* Get product_name */
	tmp = get_string_from_xpath(xml, "//device/capability/product", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "product_name", 13, tmp, 1);

	/* Get vendor_id */
	tmp = get_string_from_xpath(xml, "//device/capability/vendor/@id", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "vendor_id", 10, tmp, 1);

	/* Get vendor_name */
	tmp = get_string_from_xpath(xml, "//device/capability/vendor", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "vendor_name", 12, tmp, 1);

	/* Get driver name */
	tmp = get_string_from_xpath(xml, "//device/driver/name", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "driver_name", 12, tmp, 1);

	/* Get driver name */
	tmp = get_string_from_xpath(xml, "//device/capability/interface", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "interface_name", 15, tmp, 1);

	/* Get driver name */
	tmp = get_string_from_xpath(xml, "//device/capability/address", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "address", 8, tmp, 1);

	/* Get driver name */
	tmp = get_string_from_xpath(xml, "//device/capability/capability/@type", NULL, &retval);
	if ((tmp != NULL) && (retval > 0))
		add_assoc_string_ex(return_value, "capabilities", 11, tmp, 1);
}

/* Network functions */

/*
	Function name:	libvirt_network_define_xml
	Since version:	0.4.2
	Description:	Function is used to define a new virtual network based on the XML description
	Arguments:	@res [resource]: libvirt connection resource
			@xml [string]: XML string definition of network to be defined
	Returns:	libvirt network resource of newly defined network
*/
PHP_FUNCTION(libvirt_network_define_xml)
{
	php_libvirt_connection *conn = NULL;
	php_libvirt_network *res_net = NULL;
	virNetwork *net;
	zval *zconn;
	char *xml = NULL;
	int xml_len;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&xml,&xml_len);

	if ((net = virNetworkDefineXML(conn->conn, xml)) == NULL) {
		set_error_if_unset("Cannot define a new network");
		RETURN_FALSE;
	}

	res_net = emalloc(sizeof(php_libvirt_network));
	res_net->network = net;
	res_net->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_net->network);
	resource_change_counter(INT_RESOURCE_NETWORK, conn->conn, res_net->network, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_net, le_libvirt_network);
}

/*
	Function name:	libvirt_network_undefine
	Since version:	0.4.2
	Description:	Function is used to undefine already defined network
	Arguments:	@res [resource]: libvirt network resource
	Returns:	TRUE for success, FALSE on error
*/
PHP_FUNCTION(libvirt_network_undefine)
{
	php_libvirt_network *network = NULL;
	zval *znetwork;

	GET_NETWORK_FROM_ARGS("r",&znetwork);

	if (virNetworkUndefine(network->network) != 0)
		RETURN_FALSE;

	RETURN_TRUE;
}

/*
	Function name:	libvirt_network_get
	Since version:	0.4.1(-1)
	Description:	Function is used to get the network resource from name
	Arguments:	@res [resource]: libvirt connection resource
			@name [string]: network name string
	Returns:	libvirt network resource
*/
PHP_FUNCTION(libvirt_network_get)
{
	php_libvirt_connection *conn = NULL;
	php_libvirt_network *res_net = NULL;
	virNetwork *net;
	zval *zconn;
	char *name;
	int name_len;

	GET_CONNECTION_FROM_ARGS("rs",&zconn,&name,&name_len);

	if ((net = virNetworkLookupByName(conn->conn, name)) == NULL) {
		set_error_if_unset("Cannot get find requested network" TSRMLS_CC);
		RETURN_FALSE;
	}

	res_net = emalloc(sizeof(php_libvirt_network));
	res_net->network = net;
	res_net->conn = conn;

	DPRINTF("%s: returning %p\n", PHPFUNC, res_net->network);
	resource_change_counter(INT_RESOURCE_NETWORK, conn->conn, res_net->network, 1);
	ZEND_REGISTER_RESOURCE(return_value, res_net, le_libvirt_network);
}

/*
	Function name:	libvirt_network_get_bridge
	Since version:	0.4.1(-1)
	Description:	Function is used to get the bridge associated with the network
	Arguments:	@res [resource]: libvirt network resource
	Returns:	bridge name string
*/
PHP_FUNCTION(libvirt_network_get_bridge)
{
	php_libvirt_network *network;
	zval *znetwork;
	char *name;

	GET_NETWORK_FROM_ARGS("r",&znetwork);

	name = virNetworkGetBridgeName(network->network);

	if (name == NULL) {
		set_error_if_unset("Cannot get network bridge name" TSRMLS_CC);
		RETURN_FALSE;
	}

	RETURN_STRING(name, 1);
}

/*
	Function name:	libvirt_network_get_active
	Since version:	0.4.1(-1)
	Description:	Function is used to get the activity state of the network
	Arguments:	@res [resource]: libvirt network resource
	Returns:	1 when active, 0 when inactive, FALSE on error
*/
PHP_FUNCTION(libvirt_network_get_active)
{
	php_libvirt_network *network;
	zval *znetwork;
	int res;

	GET_NETWORK_FROM_ARGS("r",&znetwork);

	res = virNetworkIsActive(network->network);

	if (res == -1) {
		set_error_if_unset("Error getting virtual network state" TSRMLS_CC);
		RETURN_FALSE;
	}

	RETURN_LONG(res);
}

/*
	Function name:	libvirt_network_get_information
	Since version:	0.4.1(-1)
	Description:	Function is used to get the network information
	Arguments:	@res [resource]: libvirt network resource
	Returns:	network information array
*/
PHP_FUNCTION(libvirt_network_get_information)
{
	php_libvirt_network *network = NULL;
	zval *znetwork;
	int retval = 0;
	char *xml  = NULL;
	char *tmp  = NULL;
	char *tmp2 = NULL;
	char fixedtemp[32] = { 0 };

	GET_NETWORK_FROM_ARGS("r",&znetwork);

	xml=virNetworkGetXMLDesc(network->network, 0);

	if (xml==NULL) {
		set_error_if_unset("Cannot get network XML" TSRMLS_CC);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* Get name */
	tmp = get_string_from_xpath(xml, "//network/name", NULL, &retval);
	if (tmp == NULL) {
		set_error("Invalid XPath node for network name" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (retval < 0) {
		set_error("Cannot get XPath expression result for network name" TSRMLS_CC);
		RETURN_FALSE;
	}

	add_assoc_string_ex(return_value, "name", 5, tmp, 1);

	/* Get gateway IP address */
	tmp = get_string_from_xpath(xml, "//network/ip/@address", NULL, &retval);
	if (tmp == NULL) {
		set_error("Invalid XPath node for network gateway IP address" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (retval < 0) {
		set_error("Cannot get XPath expression result for network gateway IP address" TSRMLS_CC);
		RETURN_FALSE;
	}

	add_assoc_string_ex(return_value, "ip", 3, tmp, 1);

	/* Get netmask */
	tmp2 = get_string_from_xpath(xml, "//network/ip/@netmask", NULL, &retval);
	if (tmp2 == NULL) {
		set_error("Invalid XPath node for network mask" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (retval < 0) {
		set_error("Cannot get XPath expression result for network mask" TSRMLS_CC);
		RETURN_FALSE;
	}

	add_assoc_string_ex(return_value, "netmask", 8, tmp2, 1);
	add_assoc_long(return_value, "netmask_bits", (long)get_subnet_bits(tmp2));

	/* Format CIDR address representation */
	tmp[strlen(tmp) - 1] = tmp[strlen(tmp) - 1] - 1;
	snprintf(fixedtemp, sizeof(fixedtemp), "%s/%d", tmp, get_subnet_bits(tmp2));
	add_assoc_string_ex(return_value, "ip_range", 9, fixedtemp, 1);

	/* Get forwarding settings */
	tmp = get_string_from_xpath(xml, "//network/forward/@mode", NULL, &retval);
	if ((tmp == NULL) || (retval < 0))
		add_assoc_string_ex(return_value, "forwarding", 11, "None", 1);
	else
		add_assoc_string_ex(return_value, "forwarding", 11, tmp, 1);

	/* Get forwarding settings */
	tmp = get_string_from_xpath(xml, "//network/forward/@dev", NULL, &retval);
	if ((tmp == NULL) || (retval < 0))
		add_assoc_string_ex(return_value, "forward_dev", 12, "any interface", 1);
	else
		add_assoc_string_ex(return_value, "forward_dev", 12, tmp, 1);

	/* Get DHCP values */
	tmp = get_string_from_xpath(xml, "//network/ip/dhcp/range/@start", NULL, &retval);
	tmp2 = get_string_from_xpath(xml, "//network/ip/dhcp/range/@end", NULL, &retval);
	if ((retval > 0) && (tmp != NULL) && (tmp2 != NULL)) {
		add_assoc_string_ex(return_value, "dhcp_start", 11, tmp,  1);
		add_assoc_string_ex(return_value, "dhcp_end",    9, tmp2, 1);
	}
}

/*
	Function name:	libvirt_network_set_active
	Since version:	0.4.1(-1)
	Description:	Function is used to set the activity state of the network
	Arguments:	@res [resource]: libvirt network resource
	Returns:	TRUE if success, FALSE on error
*/
PHP_FUNCTION(libvirt_network_set_active)
{
	php_libvirt_network *network;
	zval *znetwork;
	int act = 0;

	GET_NETWORK_FROM_ARGS("rl",&znetwork,&act);

	if ((act != 0) && (act != 1)) {
		set_error("Invalid network activity state" TSRMLS_CC);
		RETURN_FALSE;
	}

	if (act == 1) {
		if (virNetworkCreate(network->network) == 0) {
			/* Network is up and running */
			RETURN_TRUE;
		}
		else {
			/* We don't have to set error since it's caught by libvirt error handler itself */
			RETURN_FALSE;
		}
	}

	if (virNetworkDestroy(network->network) == 0) {
		/* Network is down */
		RETURN_TRUE;
	}
	else {
		/* Caught by libvirt error handler too */
		RETURN_FALSE;
	}
}

/*
	Function name:	libvirt_network_get_xml_desc
	Since version:	0.4.1(-1)
	Description:	Function is used to get the XML description for the network
	Arguments:	@res [resource]: libvirt network resource
			@xpath [string]: optional xPath expression string to get just this entry, can be NULL
	Returns:	network XML string or result of xPath expression
*/
PHP_FUNCTION(libvirt_network_get_xml_desc)
{
	php_libvirt_network *network;
	zval *znetwork;
	char *xml = NULL;
	char *xml_out = NULL;
	char *xpath = NULL;
	char *tmp;
	int xpath_len;
	int retval = -1;

	GET_NETWORK_FROM_ARGS("r|s",&znetwork,&xpath,&xpath_len);
	if (xpath_len < 1)
	{
		xpath = NULL;
	}

	xml=virNetworkGetXMLDesc(network->network, 0);

	if (xml==NULL) {
		set_error_if_unset("Cannot get network XML" TSRMLS_CC);
		RETURN_FALSE;
	}

	tmp = get_string_from_xpath(xml, xpath, NULL, &retval);
	if ((tmp == NULL) || (retval < 0)) {
		RECREATE_STRING_WITH_E (xml_out, xml);
	} else {
		RECREATE_STRING_WITH_E (xml_out, tmp);
	}

	RETURN_STRING(xml_out, 0);
}

/*
	Function name:	libvirt_version
	Since version:	0.4.1(-1)
	Description:	Function is used to get libvirt, driver and libvirt-php version numbers. Can be used for information purposes, for version checking please use libvirt_check_version() defined below
	Arguments:	@type [string]: optional type string to identify driver to look at
	Returns:	libvirt, type (driver) and connector (libvirt-php) version numbers array
*/
PHP_FUNCTION(libvirt_version)
{
	unsigned long libVer;
	unsigned long typeVer;
	int type_len;
	char *type=NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &type,&type_len) == FAILURE) {
		set_error("Invalid arguments");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 0) {
		if (virGetVersion(&libVer,NULL,NULL) != 0)
			RETURN_FALSE;
    } else {
		if (virGetVersion(&libVer,type,&typeVer) != 0)
			RETURN_FALSE;
	}

	/* The version is returned as: major * 1,000,000 + minor * 1,000 + release. */
	array_init(return_value);

	add_assoc_long(return_value, "libvirt.release",(long)(libVer %1000));
	add_assoc_long(return_value, "libvirt.minor",(long)((libVer/1000) % 1000));
	add_assoc_long(return_value, "libvirt.major",(long)((libVer/1000000) % 1000));

	add_assoc_string_ex(return_value, "connector.version", 18, PHP_LIBVIRT_WORLD_VERSION, 1);
	add_assoc_long(return_value, "connector.major", VERSION_MAJOR);
	add_assoc_long(return_value, "connector.minor", VERSION_MINOR);
	add_assoc_long(return_value, "connector.release", VERSION_MICRO);

    if (ZEND_NUM_ARGS() > 0) {
		add_assoc_long(return_value, "type.release",(long)(typeVer %1000));
		add_assoc_long(return_value, "type.minor",(long)((typeVer/1000) % 1000));
		add_assoc_long(return_value, "type.major",(long)((typeVer/1000000) % 1000));
    }
}

/*
	Function name:	libvirt_check_version
	Since version:	0.4.1(-1)
	Description:	Function is used to check major, minor and micro (also sometimes called release) versions of libvirt-php or libvirt itself. This could useful when you want your application to support only versions of libvirt or libvirt-php higher than some version specified.
	Arguments:	@major [int]: major version number to check for
			@minor [int]: minor version number to check for
			@micro [int]: micro (also release) version number to check for
			@type [int]: type of checking, VIR_VERSION_BINDING to check against libvirt-php binding or VIR_VERSION_LIBVIRT to check against libvirt version
	Returns:	TRUE if version is equal or higher than required, FALSE if not, FALSE with error [for libvirt_get_last_error()] on unsupported version type check
*/
PHP_FUNCTION(libvirt_check_version)
{
	unsigned long libVer;
	int major = -1, minor = -1, micro = -1, type = VIR_VERSION_BINDING;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|l", &major, &minor, &micro, &type) == FAILURE) {
		set_error("Invalid arguments");
		RETURN_FALSE;
	}

	if (virGetVersion(&libVer,NULL,NULL) != 0)
		RETURN_FALSE;

	DPRINTF("%s: Checking for version %d.%d.%d of %s\n", PHPFUNC, major, minor, micro,
			(type == VIR_VERSION_BINDING) ? "php bindings" :
			((type == VIR_VERSION_LIBVIRT) ? "libvirt" : "unknown"));

	if (type == VIR_VERSION_BINDING) {
		if ((VERSION_MAJOR > major) ||
			((VERSION_MAJOR == major) && (VERSION_MINOR > minor)) ||
			((VERSION_MAJOR == major) && (VERSION_MINOR == minor) &&
			(VERSION_MICRO >= micro)))
				RETURN_TRUE;
	}
	else
	if (type == VIR_VERSION_LIBVIRT) {
		if ((((libVer/1000000) % 1000) > major) ||
			((((libVer/1000000) % 1000) == major) && (((libVer/1000) % 1000) > minor)) ||
			((((libVer/1000000) % 1000) == major) && (((libVer/1000) % 1000) == minor) &&
			((libVer %1000) >= micro)))
				RETURN_TRUE;
	}
	else
		set_error("Invalid version type" TSRMLS_CC);

	RETURN_FALSE;
}

/*
	Function name:		libvirt_has_feature
	Since version:		0.4.1(-3)
	Description:		Function to check for feature existence for working libvirt instance
	Arguments:		@name [string]: feature name
	Returns:		TRUE if feature is supported, FALSE otherwise
*/
PHP_FUNCTION(libvirt_has_feature)
{
	char *name = NULL;
	int name_len = 0;
	char *binary = NULL;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		set_error("Invalid argument");
		RETURN_FALSE;
	}

	binary = get_feature_binary(name);
	ret = (binary != NULL);
	free(binary);

	if (ret)
		RETURN_TRUE;

	RETURN_FALSE;
}

/*
	Function name:		libvirt_get_iso_images
	Since version:		0.4.1(-3)
	Description:		Function to get the ISO images on path and return them in the array
	Arguments:		@path [string]: string of path where to look for the ISO images
	Returns:		ISO image array on success, FALSE otherwise
*/
PHP_FUNCTION(libvirt_get_iso_images)
{
	char *path = NULL;
	int path_len = 0;
	struct dirent *entry;
	DIR *d = NULL;
	int num = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &path, &path_len) == FAILURE) {
		set_error("Invalid argument");
		RETURN_FALSE;
	}

	if (LIBVIRT_G(iso_path_ini))
		path = strdup( LIBVIRT_G(iso_path_ini) );

	if ((path == NULL) || (path[0] != '/')) {
		set_error("Invalid argument, path must be set and absolute (start by slash character [/])");
		RETURN_FALSE;
	}

	DPRINTF("%s: Getting ISO images on path %s\n", PHPFUNC, path);

        if ((d = opendir(path)) != NULL) {
		array_init(return_value);
		while ((entry = readdir(d)) != NULL) {
			if (strcasecmp(entry->d_name + strlen(entry->d_name) - 4, ".iso") == 0) {
				add_next_index_string(return_value, entry->d_name, 1);
				num++;
			}
		}
		closedir(d);
	}

	if (num == 0)
		RETURN_FALSE;
}

/*
	Function name:		libvirt_print_binding_resources
	Since version:		0.4.2
	Description:		Function to print the binding resources, although the resource information are printed, they are returned in the return_value
	Arguments:		None
	Returns:		bindings resource information
*/
PHP_FUNCTION(libvirt_print_binding_resources)
{
	int binding_resources_count = 0;
	resource_info *binding_resources;
	char tmp[256] = { 0 };
	int i;

	binding_resources_count = LIBVIRT_G(binding_resources_count);
	binding_resources = LIBVIRT_G(binding_resources);

	array_init(return_value);
	for (i = 0; i < binding_resources_count; i++) {
		if (binding_resources[i].overwrite == 0) {
			if (binding_resources[i].conn != NULL)
				snprintf(tmp, sizeof(tmp), "Libvirt %s resource at 0x%llx (connection %p)", translate_counter_type(binding_resources[i].type),
						binding_resources[i].mem, binding_resources[i].conn);
			else
				snprintf(tmp, sizeof(tmp), "Libvirt %s resource at 0x%llx", translate_counter_type(binding_resources[i].type),
						binding_resources[i].mem);
			add_next_index_string(return_value, tmp, 1);
		}
	}

	if (binding_resources_count == 0)
		RETURN_FALSE;
}

#ifdef DEBUG_SUPPORT
/*
	Function name:		libvirt_logfile_set
	Since version:		0.4.2
	Description:		Function to set the log file for the libvirt module instance
	Arguments:		@filename [string]: log filename or NULL to disable logging
				@maxsize [long]: optional maximum log file size argument in KiB, default value can be found in PHPInfo() output
	Returns:		TRUE if log file has been successfully set, FALSE otherwise
*/
PHP_FUNCTION(libvirt_logfile_set)
{
	char *filename = NULL;
	long maxsize = DEFAULT_LOG_MAXSIZE;
	int filename_len = 0;
	int err;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &filename, &filename_len, &maxsize) == FAILURE) {
		set_error("Invalid argument");
		RETURN_FALSE;
	}

	if ((filename == NULL) || (filename_len < 1) || (strcasecmp(filename, "null") == 0))
		err = set_logfile(NULL, 0);
	else
		err = set_logfile(filename, maxsize);

	if (err < 0) {
		char tmp[1024] = { 0 };
		snprintf(tmp, sizeof(tmp), "Cannot set the log file to %s, error code = %d (%s)", filename, err, strerror(-err));
		set_error(tmp);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
#endif

