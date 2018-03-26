#ifndef __KLOG_H__
#define __KLOG_H__

#include <linux/kernel.h>

extern const char* MOD_NAME;

#define LOG_DEBUG(fmt, args...)                                                \
  printk(KERN_DEBUG "%s @ %s(): " fmt "\n", MOD_NAME, __func__, ##args)
#define LOG_INFO(fmt, args...)                                                 \
  printk(KERN_INFO "%s @ %s(): " fmt "\n", MOD_NAME, __func__, ##args)
#define LOG_ERROR(fmt, args...)                                                  \
  printk(KERN_ERR "%s @ %s(): " fmt "\n", MOD_NAME, __func__, ##args)

#endif