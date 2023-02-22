#include <zephyr/bluetooth/mesh.h>
#include "mesh/net.h"
#include "soil_srv.h"

static int bt_mesh_soil_srv_report(struct bt_mesh_soil_srv *srv, struct bt_mesh_soil_report soil)
{
    printk("bt_mesh_soil_srv_report\n");
    bt_mesh_model_msg_init(&srv->pub_msg, BT_MESH_SOIL_OP_SOIL_REPORT);
	net_buf_simple_add_mem(&srv->pub_msg, &soil.humidity, sizeof(uint8_t));
	net_buf_simple_add_mem(&srv->pub_msg, &soil.temperature, sizeof(uint8_t));

    return bt_mesh_model_publish(srv->model);
}

static int bt_mesh_soil_srv_update(struct bt_mesh_model *model)
{
    printk("bt_mesh_soil_srv_update\n");
    bt_mesh_model_msg_init(model->pub->msg, BT_MESH_SOIL_OP_SOIL_REPORT);

    return 0;
}

static int bt_mesh_soil_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_soil_srv *srv = model->user_data;

	srv->model = model;

	net_buf_simple_init_with_data(&srv->pub_msg, srv->buf, sizeof(srv->buf));
	
    srv->pub.msg = &srv->pub_msg;
    srv->pub.update = bt_mesh_soil_srv_update;

	printk("Init Soil Model\n");
	return 0;
}

const struct bt_mesh_model_cb _bt_mesh_soil_srv_cb = {
	.init = bt_mesh_soil_srv_init,
};