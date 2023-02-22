#include <zephyr/bluetooth/mesh.h>
#include "soil_cli.h"
#include "mesh/net.h"

static int handle_message_report(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct net_buf_simple *buf)
{
	printk("handle_message_report\n");

	struct bt_mesh_soil_cli *cli = model->user_data;
	struct bt_mesh_soil_report soil;
	soil.humidity = net_buf_simple_pull_u8(buf);
	soil.temperature = net_buf_simple_pull_u8(buf);

	if (cli->handlers->report) {
		cli->handlers->report(cli, ctx, soil);
	}

	return 0;
}

const struct bt_mesh_model_op _bt_mesh_soil_cli_op[] = {
	{
		BT_MESH_SOIL_OP_SOIL_REPORT, BT_MESH_LEN_EXACT(1), handle_message_report
	},
	BT_MESH_MODEL_OP_END,
};

static int bt_mesh_soil_cli_init(struct bt_mesh_model *model)
{
	printk("bt_mesh_soil_cli_init\n");
	struct bt_mesh_soil_cli *cli = model->user_data;

	cli->model = model;

	net_buf_simple_init_with_data(&cli->pub_msg, cli->buf, sizeof(cli->buf));

	cli->pub.msg = &cli->pub_msg;

	return 0;
}

const struct bt_mesh_model_cb _bt_mesh_soil_cli_cb = {
	.init = bt_mesh_soil_cli_init,
};