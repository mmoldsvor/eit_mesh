#ifndef MODEL_HANDLER_H__
#define MODEL_HANDLER_H__

#include <zephyr/bluetooth/mesh.h>
#include "soil.h"

const struct bt_mesh_comp *model_handler_init(void);

void soil_report(struct bt_mesh_soil_report soil);

#endif
