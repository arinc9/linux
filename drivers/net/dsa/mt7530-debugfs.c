// SPDX-License-Identifier: GPL-2.0-only
#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <net/dsa.h>

#include "mt7530.h"

static u32 reg_address = 0;

static ssize_t reg_address_read(struct file *filp, char __user *buffer,
				size_t count, loff_t *ppos)
{
	char result[10];

	snprintf(result, sizeof(result), "%08x\n", reg_address);

	return simple_read_from_buffer(buffer, count, ppos, result,
				       strlen(result));
}

static ssize_t reg_address_write(struct file *filp, const char __user *buffer,
				 size_t count, loff_t *ppos)
{
	int ret;

	ret = kstrtou32_from_user(buffer, count, 16, &reg_address);
	if (ret)
		return ret;

	return count;
}

static ssize_t reg_value_read(struct file *filp, char __user *buffer,
			      size_t count, loff_t *ppos)
{
	struct mt7530_priv *priv = filp->private_data;
	char result[10];
	u32 reg_value;

	reg_value = mt7530_read(priv, reg_address);

	snprintf(result, sizeof(result), "%08x\n", reg_value);

	return simple_read_from_buffer(buffer, count, ppos, result,
	                               strlen(result));
}

static ssize_t reg_value_write(struct file *filp, const char __user *buffer,
			       size_t count, loff_t *ppos)
{
	struct mt7530_priv *priv = filp->private_data;
	u32 reg_value;
	int ret;

	ret = kstrtou32_from_user(buffer, count, 16, &reg_value);
	if (ret)
		return ret;

	mt7530_write(priv, reg_address, reg_value);

	return count;
}

static const struct file_operations reg_address_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = reg_address_read,
	.write = reg_address_write,
};

static const struct file_operations reg_value_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = reg_value_read,
	.write = reg_value_write,
};

void mt7530_debugfs_init(struct mt7530_priv *priv)
{
	struct dentry *dir;

	dir = debugfs_lookup("mt7530", 0);
	if (dir == NULL)
		dir = debugfs_create_dir("mt7530", 0);

	debugfs_create_file("reg_address", 0644, dir, priv, &reg_address_fops);
	debugfs_create_file("reg_value", 0644, dir, priv, &reg_value_fops);
}

MODULE_AUTHOR("Arınç ÜNAL <arinc.unal@arinc9.com>");
MODULE_DESCRIPTION("DebugFS for MT753X DSA subdriver");
MODULE_LICENSE("GPL");
