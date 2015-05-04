#include <stdlib.h>
#include <IP2Location.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "vcc_if.h"
#include "vmod_ip2location.h"

#define VMOD_LOG(...) WSP(sp, SLT_VCL_Log, __VA_ARGS__);

void
i2pl_free(void *d)
{
  ip2location_data_t *data = d;

  if (data->ip2l_handle != NULL)
    IP2Location_close(data->ip2l_handle);
}

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
        ip2location_data_t *data = calloc(1, sizeof(*data));
        /* we test for the validity of the handle at each call */
        data->ip2l_handle = IP2Location_open(IP2LOCATION_DB_PATH);
        priv->priv = data;
        priv->free = i2pl_free;
	return (0);
}

const char *
vmod_country_short(struct sess *sp, struct vmod_priv *priv, const char *ip)
{
	IP2LocationRecord *r;
	IP2Location *handle;
	const char *code = NULL;

        handle = ((ip2location_data_t *)priv->priv)->ip2l_handle;
        if (handle != NULL) {
		r = IP2Location_get_country_short(handle, (char *)ip);
		code = WS_Dup(sp->wrk->ws, r->country_short);
        	IP2Location_free_record(r);
	} else {
		VMOD_LOG("ERROR: IP2Location database failed to load");
		code = WS_Dup(sp->wrk->ws, "-");
	}

	return (code);
}

const char *
vmod_region(struct sess *sp, struct vmod_priv *priv, const char *ip)
{
	IP2LocationRecord *r;
	IP2Location *handle;
	const char *code = NULL;

        handle = ((ip2location_data_t *)priv->priv)->ip2l_handle;
        if (handle != NULL) {
		r = IP2Location_get_region(handle, (char *)ip);
		code = WS_Dup(sp->wrk->ws, r->region);
        	IP2Location_free_record(r);
	} else {
		VMOD_LOG("ERROR: IP2Location database failed to load");
		code = WS_Dup(sp->wrk->ws, "-");
	}

	return (code);
}
