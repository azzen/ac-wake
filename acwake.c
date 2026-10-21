// SPDX-License-Identifier: GPL-2.0
/*
 * wake the system from suspend-to-idle when the AC adapter is plugged in.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/pm_wakeup.h>
#include <linux/power_supply.h>
#include <linux/suspend.h>

static bool wake_on_unplug;
module_param(wake_on_unplug, bool, 0644);
MODULE_PARM_DESC(wake_on_unplug,
                 "Also wake when the AC adapter is unplugged (default: N)");

static char *psy_name = "AC";
module_param(psy_name, charp, 0444);
MODULE_PARM_DESC(
    psy_name, "power_supply device the wakeup is attributed to (default: AC)");

static int acwake_acpi_event(struct notifier_block *nb, unsigned long type,
                             void *data) {
  struct acpi_bus_event *ev = data;
  struct power_supply *psy;
  const char *state;
  bool online;

  if (strcmp(ev->device_class, ACPI_AC_CLASS) != 0)
    return NOTIFY_DONE;

  /* ev->data carries ac->state: 1 online, 0 offline, -1 unknown. */
  online = ev->data == 1;
  state = online ? "online" : "offline";

  if (!online && !wake_on_unplug)
    return NOTIFY_DONE;

  /* Only act while the system is suspending or asleep. */
  if (pm_suspend_target_state == PM_SUSPEND_ON) {
    pr_debug("AC adapter %s while awake, nothing to do\n", state);
    return NOTIFY_DONE;
  }

  psy = power_supply_get_by_name(psy_name);
  if (psy) {
    if (device_may_wakeup(&psy->dev)) {
      pm_wakeup_hard_event(&psy->dev);
      pr_info("AC adapter %s during suspend: waking up\n", state);
    } else {
      pr_info("AC adapter %s during suspend, but wakeup is disabled on %s\n",
              state, psy_name);
    }
    power_supply_put(psy);
  } else {
    pm_system_wakeup();
    pr_info("AC adapter %s during suspend: waking up (no %s power_supply)\n",
            state, psy_name);
  }

  return NOTIFY_OK;
}

static struct notifier_block acwake_nb = {
    .notifier_call = acwake_acpi_event,
};

static int __init acwake_init(void) {
  int ret;

  ret = register_acpi_notifier(&acwake_nb);
  if (ret)
    return ret;

  pr_info("loaded: AC adapter plug-in will wake the system from suspend\n");
  return 0;
}

static void __exit acwake_exit(void) { unregister_acpi_notifier(&acwake_nb); }

module_init(acwake_init);
module_exit(acwake_exit);

MODULE_AUTHOR("azzen");
MODULE_DESCRIPTION(
    "Wake from suspend-to-idle when the AC adapter is plugged in");
MODULE_LICENSE("GPL");
