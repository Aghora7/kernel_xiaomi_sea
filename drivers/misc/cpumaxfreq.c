#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#define cpumaxfreq_proc_name "cpumaxfreq"
static struct proc_dir_entry  *cpumaxfreq_proc;
#define CPU_PRESENT   "/sys/devices/system/cpu/present"

int find_symbol_form_string(char *string, char symbol)
{
	int i = 0;
	while (!(string[i] == symbol))
	++i;
	return i+1;
}

int read_file(char *file_path, char *buf, int size)
{

	struct file *file_p = NULL;
	struct filename *filename;
	mm_segment_t old_fs;
	loff_t pos;
	int ret;

	filename = getname_kernel(file_path);
	file_p = file_open_name(filename, O_RDONLY, 0);
	if (IS_ERR(file_p)) {
		pr_err ("%s fail to open file \n", __func__);
		return  PTR_ERR(file_p); }
	else {
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		pos = 0;
		ret = vfs_read(file_p, buf, size, &pos);
		filp_close(file_p, NULL);
		set_fs(old_fs);
		file_p = NULL;
	}

	return ret;
}
int get_core_count(void)
{
    char buf[8] = {0};
	int symbol_position = 0;
	int core_count = 0;
	read_file(CPU_PRESENT, buf, sizeof(buf));
	symbol_position = find_symbol_form_string(buf, '-');

	core_count = buf[symbol_position]-'0'+1;

	return core_count;



}
void read_cpumaxfreq(char *cpumaxfreq_buf)
{
	uint16_t i = 0;
	char buf[16] = {0};
	char path[128] = {0};
	long cpumaxfreq = 0;
	int core_count = 0;
	core_count = get_core_count();
	//find max freq cpu
	while (i < core_count) {
		memset(buf, 0, sizeof(buf));
		snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", i);
		read_file(path, buf, sizeof(buf));
		if (simple_strtoul(buf, NULL, 0) > cpumaxfreq)
			cpumaxfreq = simple_strtoul(buf, NULL, 0);

		++i;
	}
	//get max freq
	snprintf(cpumaxfreq_buf, 16, "%lu.%02lu", (uint16_t)(cpumaxfreq/1000000),
		(uint16_t)((cpumaxfreq%1000000)/10000));
}

static int cpumaxfreq_show(struct seq_file *file, void *data)
{
	char cpumaxfreq_buf[16];
#if 0
	char *cpumaxfreq_buf = NULL;
	cpumaxfreq_buf = kmalloc(sizeof(*cpumaxfreq_buf)*cpu_num*32, GFP_KERNEL);//32bytes for every cpu
	if (IS_ERR(cpumaxfreq_buf)) {
		pr_err("%s cpumaxfreq_buf kmalloc fail.\n", __func__);
		return PTR_ERR(cpumaxfreq_buf);
	}
	memset(cpumaxfreq_buf, 0, sizeof(*cpumaxfreq_buf)*cpu_num*32);
#endif
	memset(cpumaxfreq_buf, 0, sizeof(cpumaxfreq_buf));
	read_cpumaxfreq(cpumaxfreq_buf);
	seq_printf(file, "%s\n", cpumaxfreq_buf);
#if 0
	if (!cpumaxfreq_buf)
	kfree(cpumaxfreq_buf);
#endif
	return 0;
}
static int cpumaxfreq_open(struct inode *inode, struct file *file)
{
	return single_open(file, cpumaxfreq_show, inode->i_private);
}
static const struct file_operations cpumaxfreq_ops = {
	    .owner = THIS_MODULE,
		.open = cpumaxfreq_open,
		.read = seq_read,
};
int create_fs(void)
{
	/*proc/cpumaxfreq*/
	long rc = 1;
	cpumaxfreq_proc = proc_create(cpumaxfreq_proc_name, 0444, NULL, &cpumaxfreq_ops);
	if (IS_ERR(cpumaxfreq_proc)) {
		pr_err("%s cpumaxfreq proc create fail.\n", __func__);
		rc = PTR_ERR(cpumaxfreq_proc);
	}

	return rc;
}

static int __init mi_fs_init(void)
{
	/* create fs*/
	create_fs();

	return 0;
}


late_initcall(mi_fs_init); //after module_init
