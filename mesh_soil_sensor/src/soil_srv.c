#include <zephyr/bluetooth/mesh.h>
#include "mesh/net.h"
#include "soil_srv.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>


int bt_mesh_soil_srv_report(struct bt_mesh_soil_srv *srv, struct bt_mesh_soil_report soil)
{
    printk("bt_mesh_soil_srv_report\n");
    bt_mesh_model_msg_init(&srv->pub_msg, BT_MESH_SOIL_OP_SOIL_REPORT);
	net_buf_simple_add_mem(&srv->pub_msg, &soil.humidity, sizeof(soil.humidity));
	net_buf_simple_add_mem(&srv->pub_msg, &soil.temperature, sizeof(soil.temperature));

    return bt_mesh_model_publish(srv->model);
}

// static int bt_mesh_soil_srv_update(struct bt_mesh_model *model)
// {
// 	struct bt_mesh_soil_srv *soil = model->user_data;
//     printk("bt_mesh_soil_srv_update\n");
//     bt_mesh_model_msg_init(model->pub->msg, BT_MESH_SOIL_OP_SOIL_REPORT);
// 	net_buf_simple_add_u8(model->pub->msg, 1);
// 	net_buf_simple_add_u8(model->pub->msg, 2);

//     return 0;
// }

static int bt_mesh_soil_srv_init(struct bt_mesh_model *model)
{
	struct bt_mesh_soil_srv *srv = model->user_data;

	srv->model = model;

	net_buf_simple_init_with_data(&srv->pub_msg, srv->buf, sizeof(srv->buf));
	
    srv->pub.msg = &srv->pub_msg;
    // srv->pub.update = bt_mesh_soil_srv_update;

	printk("Init Soil Model\n");
	return 0;
}

const struct bt_mesh_model_cb _bt_mesh_soil_srv_cb = {
	.init = bt_mesh_soil_srv_init,
};
