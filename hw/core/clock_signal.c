#include "hw/clock_signal.h"
#include "qemu-common.h"
#include "qemu/notify.h"
#include "qemu/log.h"
#include "qapi-types.h"
#include "qapi-visit.h"
#include "qapi/visitor.h"



/* Helper functions for creating and modifying clock signal objects */
clkfreq clock_signal_get_output_freq(ClockSignalDevice *clk)
{
    return object_property_get_int(OBJECT(clk), "output-freq", NULL);
}

void clock_signal_add_notify(ClockSignalDevice *clk,
                                ClockSignalDeviceCB callback,
                                void *data)
{
    ClockSignalDeviceClass *clkclass = CLOCK_SIGNAL_DEVICE_GET_CLASS(clk);
    clkclass->add_notify(clk, callback, data);
}

void clock_signal_set_enabled(ClockSignalDevice *clk, bool enabled)
{
    object_property_set_bool(OBJECT(clk), enabled, "enabled", NULL);
}

bool clock_signal_is_enabled(ClockSignalDevice *clk)
{
    return object_property_get_bool(OBJECT(clk), "enabled", NULL);
}

void clock_signal_set_max_freq(ClockSignalDevice *clk, clkfreq max_freq)
{
    object_property_set_int(OBJECT(clk), max_freq, "max-freq", NULL);
}

ClockSignalSource *new_clock_signal_source(Object *parent, const char *name, clkfreq freq, bool enabled)
{
    DeviceState *dev = qdev_create(NULL, TYPE_CLOCK_SIGNAL_SOURCE);
    object_property_add_child(parent, name, OBJECT(dev), NULL);
    clock_signal_set_enabled(CLOCK_SIGNAL_DEVICE(dev), enabled);
    clock_signal_source_set_freq(CLOCK_SIGNAL_SOURCE(dev), freq);
    qdev_init_nofail(dev);
    return CLOCK_SIGNAL_SOURCE(dev);
}

void clock_signal_source_set_freq(ClockSignalSource *clk, clkfreq freq)
{
    object_property_set_int(OBJECT(clk), freq, "source-freq", NULL);
}

ClockTreeNode *new_clock_tree_node(
                         Object *parent, const char *name,
                         ClockSignalDevice *input_clock,
                         uint32_t mul, uint32_t div,
                         bool enabled)
{
    DeviceState *dev = qdev_create(NULL, TYPE_CLOCK_TREE_NODE);
    object_property_add_child(parent, name, OBJECT(dev), NULL);
    clock_signal_set_enabled(CLOCK_SIGNAL_DEVICE(dev), enabled);
    clock_node_set_scale(CLOCK_TREE_NODE(dev), mul, div);
    if(input_clock) {
        clock_node_set_input_clock(CLOCK_TREE_NODE(dev), input_clock);
    }
    qdev_init_nofail(dev);
    return CLOCK_TREE_NODE(dev);
}

void clock_node_set_input_clock(ClockTreeNode *clk,
                                 ClockSignalDevice *input_clock)
{
    object_property_set_link(
            OBJECT(clk), OBJECT(input_clock), "input-clock", NULL);
    object_property_set_bool(
            OBJECT(clk), true, "input-clock-changed", NULL);
}

void clock_node_set_scale(ClockTreeNode *clk, uint32_t mul, uint32_t div)
{
    object_property_set_int(OBJECT(clk), mul, "multiplier", NULL);
    object_property_set_int(OBJECT(clk), div, "divider", NULL);
}




struct ClockSignalDevice {
    /*< private >*/
    DeviceState qdev;
    /*< public >*/

    clkfreq freq, output_freq, max_output_freq;
    bool output_enabled;
    NotifierList output_change_notifiers;
};

static void clk_sig_dev_recalc_output_freq(ClockSignalDevice *clk)
{
    clkfreq old_output_freq = clk->output_freq;
    clk->output_freq = clk->output_enabled ? clk->freq : 0;
    if(clk->output_freq != old_output_freq) {
        notifier_list_notify(&clk->output_change_notifiers, clk);
    }
}

static void clk_sig_dev_check_max_freq(ClockSignalDevice *clk)
{
    if(clk->max_output_freq) {
        if(clk->output_freq > clk->max_output_freq) {
            qemu_log_mask(LOG_GUEST_ERROR,
                    "Clock Signal Device: Clock %s exceeded its maximum"
                    "frequency: output freq=%llu max freq=%llu\n",
                    object_get_canonical_path(OBJECT(clk)),
                    (unsigned long long)clk->output_freq,
                    (unsigned long long)clk->max_output_freq);
        }
    }
}

/* This is essentially a protected method.  This is to be used by
 * derived classes to set the clock's frequency.
 */
static void clk_sig_dev_set_freq(ClockSignalDevice *clk, clkfreq freq)
{
    clk->freq = freq;
    clk_sig_dev_check_max_freq(clk);
    clk_sig_dev_recalc_output_freq(clk);
}

static clkfreq clk_sig_dev_get_output_freq(ClockSignalDevice *clk)
{
    return clk->output_freq;
}

typedef struct ClockSignalNotifier {
    Notifier n;
    ClockSignalDeviceCB *callback;
    void *data;
} ClockSignalNotifier;

static void clk_sig_dev_output_change_notify_handler(Notifier *notifier, void *data)
{
    ClockSignalDevice *clk = (ClockSignalDevice *)data;
    ClockSignalNotifier *csn = DO_UPCAST(ClockSignalNotifier, n, notifier);
    csn->callback(clk, clk->output_freq, csn->data);
}

static void clk_sig_dev_add_notify(ClockSignalDevice *clk,
                                   ClockSignalDeviceCB *callback,
                                   void *data)
{
    ClockSignalNotifier *csn = g_malloc0(sizeof(ClockSignalNotifier));
    csn->n.notify = clk_sig_dev_output_change_notify_handler;
    csn->callback = callback;
    csn->data = data;
    notifier_list_add(&clk->output_change_notifiers, &csn->n);
}

static void clk_sig_dev_get_output_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint64(v, &clk->output_freq, name, errp);
}

static void clk_sig_dev_get_max_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint64(v, &clk->max_output_freq, name, errp);
}

static void clk_sig_dev_set_max_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint64(v, &clk->max_output_freq, name, errp);
    clk_sig_dev_check_max_freq(clk);
}

static bool clk_sig_dev_get_enabled_prop(Object *obj, Error **errp)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    return clk->output_enabled;
}

static void clk_sig_dev_set_enabled_prop(Object *obj, bool value, Error **errp)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    clk->output_enabled = value;
    clk_sig_dev_recalc_output_freq(clk);
}

static void clk_sig_dev_instance_init(Object *obj)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    clk->freq = 0;
    clk->output_enabled = true;
    clk->output_freq = 0;
    clk->max_output_freq = 0;
    notifier_list_init(&clk->output_change_notifiers);

    object_property_add(obj, "output-freq", "int",
                        clk_sig_dev_get_output_freq_prop,
                        NULL,
                        NULL, NULL, NULL);
    object_property_add(obj, "max-freq", "int",
                        clk_sig_dev_get_max_freq_prop,
                        clk_sig_dev_set_max_freq_prop,
                        NULL, NULL, NULL);
    object_property_add_bool(obj, "enabled",
                             clk_sig_dev_get_enabled_prop,
                             clk_sig_dev_set_enabled_prop,
                             NULL);
}

static void clk_sig_dev_class_init(ObjectClass *klass, void *data)
{
    ClockSignalDeviceClass *clkclass = CLOCK_SIGNAL_DEVICE_CLASS(klass);
    clkclass->get_output_freq = clk_sig_dev_get_output_freq;
    clkclass->add_notify = clk_sig_dev_add_notify;
}

static const TypeInfo clock_signal_device_info = {
    .name = TYPE_CLOCK_SIGNAL_DEVICE,
    .parent = TYPE_DEVICE,
    .instance_size = sizeof(ClockSignalDevice),
    .instance_init = clk_sig_dev_instance_init,
    .abstract = true,
    .class_size = sizeof(ClockSignalDeviceClass),
    .class_init = clk_sig_dev_class_init,
};






static void clk_sig_src_get_source_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockSignalDevice *clk = CLOCK_SIGNAL_DEVICE(obj);
    visit_type_uint64(v, &clk->freq, name, errp);
}

static void clk_sig_src_set_source_freq_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    clkfreq new_freq;
    visit_type_uint64(v, &new_freq, name, errp);
    clk_sig_dev_set_freq(CLOCK_SIGNAL_DEVICE(obj), new_freq);
}

static void clk_sig_src_instance_init(Object *obj)
{
    object_property_add(obj, "source-freq", "int",
                        clk_sig_src_get_source_freq_prop,
                        clk_sig_src_set_source_freq_prop,
                        NULL, NULL, NULL);
}

static const TypeInfo clock_signal_source_info = {
    .name = TYPE_CLOCK_SIGNAL_SOURCE,
    .parent = TYPE_CLOCK_SIGNAL_DEVICE,
    .instance_size = sizeof(ClockSignalDevice),
    .instance_init = clk_sig_src_instance_init,
    .class_size = sizeof(ClockSignalDeviceClass),
};




struct ClockTreeNode {
    /*< private >*/
    ClockSignalDevice clk;
    /*< public >*/

    // TODO: Need to work on naming of these variables
    uint32_t multiplier, divisor;

    ClockSignalDevice *input_clock;
};

static void clk_sig_dev_get_mul_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockTreeNode *clk = CLOCK_TREE_NODE(obj);
    visit_type_uint32(v, &clk->multiplier, name, errp);
}

static void clk_sig_dev_set_mul_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockTreeNode *clk = CLOCK_TREE_NODE(obj);
    visit_type_uint32(v, &clk->multiplier, name, errp);
}

static void clk_sig_dev_get_div_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockTreeNode *clk = CLOCK_TREE_NODE(obj);
    visit_type_uint32(v, &clk->divisor, name, errp);
}

static void clk_sig_dev_set_div_prop(Object *obj, struct Visitor *v, void *opaque,
                                 const char *name, struct Error **errp)
{
    ClockTreeNode *clk = CLOCK_TREE_NODE(obj);
    visit_type_uint32(v, &clk->divisor, name, errp);
}

static bool clk_sig_dev_get_input_clock_changed_prop(Object *obj, Error **errp)
{
    return false;
}

static void clk_tree_node_propagate_input_clock_freq(ClockTreeNode *clk)
{
    ClockSignalDevice *input_clk = clk->input_clock;
    clkfreq new_input_freq = clock_signal_get_output_freq(input_clk);
    clkfreq new_freq = muldiv64(new_input_freq, clk->multiplier, clk->divisor);
    clk_sig_dev_set_freq(CLOCK_SIGNAL_DEVICE(clk), new_freq);
}

static void clk_tree_node_input_clock_freq_changed(
        ClockSignalDevice *clk,
        clkfreq new_output_freq,
        void *data)
{
    clk_tree_node_propagate_input_clock_freq((ClockTreeNode *)data);
}

static void clk_sig_dev_set_input_clock_changed_prop(Object *obj, bool value, Error **errp)
{
    ClockTreeNode *clk = CLOCK_TREE_NODE(obj);
    if(value) {
        clk_tree_node_propagate_input_clock_freq(clk);
        clock_signal_add_notify(
                clk->input_clock,
                clk_tree_node_input_clock_freq_changed,
                (void *)clk);
    }
}

static void clk_tree_node_instance_init(Object *obj)
{
    ClockTreeNode *clk = CLOCK_TREE_NODE(obj);
    clk->multiplier = 1;
    clk->divisor = 1;
    clk->input_clock = NULL;

    object_property_add(obj, "multiplier", "int",
                        clk_sig_dev_get_mul_prop,
                        clk_sig_dev_set_mul_prop,
                        NULL, NULL, NULL);
    object_property_add(obj, "divider", "int",
                        clk_sig_dev_get_div_prop,
                        clk_sig_dev_set_div_prop,
                        NULL, NULL, NULL);
    // We need to be able to trigger the frequency to be updated when
    // the input clock link is changed.  However, the link functionality
    // doesn't seem to allow custom setters.  For now we provide a
    // property to do so, but long-term need to figure out how to use
    // the link only.
    object_property_add_link(obj, "input-clock", TYPE_CLOCK_SIGNAL_DEVICE,
                             (Object **)&clk->input_clock, NULL);
    object_property_add_bool(obj, "input-clock-changed",
                             clk_sig_dev_get_input_clock_changed_prop,
                             clk_sig_dev_set_input_clock_changed_prop,
                             NULL);
}

static const TypeInfo clock_tree_node_info = {
    .name = TYPE_CLOCK_TREE_NODE,
    .parent = TYPE_CLOCK_SIGNAL_DEVICE,
    .instance_size = sizeof(ClockTreeNode),
    .instance_init = clk_tree_node_instance_init,
    .class_size = sizeof(ClockSignalDeviceClass),
};




static void clock_signal_register_types(void)
{
    type_register_static(&clock_signal_device_info);
    type_register_static(&clock_signal_source_info);
    type_register_static(&clock_tree_node_info);
}

type_init(clock_signal_register_types)
