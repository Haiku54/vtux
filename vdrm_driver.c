#include <linux/slab.h>

#include <drm/drm_drv.h>
#include <drm/drm_gem.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_device.h>

#include "vdrm_driver.h"
#include "vdrm_device.h"



DEFINE_DRM_GEM_FOPS(vdrm_fops);

struct drm_driver vdrm_drv = {
	.driver_features = DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC,
	.name = "vdrm",
	.desc = "vitrual drm",
	.date = "20221016",
	.major = 1,
	.minor = 0,
	.fops = &vdrm_fops,
};


struct vdrm_driver *vdrm_driver_init(struct device *parent) {
	struct vdrm_driver *drv;

	drv = kzalloc(sizeof(struct vdrm_driver), GFP_KERNEL);
	if (IS_ERR(drv)) {
		printk("failed to allocate memory for vdrm driver, what: %li\n", PTR_ERR(drv));
		return NULL;
	}

	drv->drm_drv = &vdrm_drv;
	drv->parent = parent;

	drv->drm_dev = vdrm_device_init(drv->drm_drv, drv->parent);
	if (!drv->drm_dev) {
		printk("failed to initialize vdrm device container\n");
		kfree(drv);
		return NULL;
	}

	return drv;
}
/*
struct vdrm_driver *vdrm_driver_init(struct device *parent) {
	int err = 0;
	//struct device *dev;
	struct vdrm_driver *drv;


	//devm_drm_dev_alloc (and any other drm dev alloc function) will fail without parent device
	drv = devm_drm_dev_alloc(parent, &vdrm_drv, struct vdrm_driver, drm_dev);
	if (IS_ERR(drv)) {
		printk("failed to create vdrm_driver, what: %li\n", PTR_ERR(drv));
		goto exit_init;
	}

	drv->drm_drv = &vdrm_drv;
	drv->parent = parent;

	
	//minimum requirement to register drm device
	err = drmm_mode_config_init(&drv->drm_dev);
	if (err) {
		printk("failed to initialize drm device mode config, what: %d\n", err);
		return NULL;
	}

	//register the drm device to the system and userspace
	err = drm_dev_register(&drv->drm_dev, 0);
	if (err) {
		printk("failed to register drm device, what: errno %d\n", err);
		goto clean_and_exit;
	}

	return drv;
clean_and_exit:
	kfree(drv);
exit_init:
	return NULL;
}

*/
void vdrm_driver_clean(struct vdrm_driver *drv) {
	vdrm_device_clean(drv->drm_dev);
	kfree(drv);
}