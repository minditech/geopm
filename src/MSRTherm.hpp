#ifndef MINDI_MSR_THERM
#define MINDI_MSR_THERM

#include "cpuid.h"
#include "msr_core.h"
#include "msr_thermal.h"
#include "libmsr_error.h"

extern "C" {
    void* libmsr_malloc(uint64_t);
} 

struct MSRTherm {

    static void init_pkg_therm_stat(struct pkg_therm_stat *pts) {
        uint64_t sockets = num_sockets();

        pts->raw = (uint64_t **) libmsr_malloc(sockets * sizeof(uint64_t *));
        pts->status = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->status_log = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->PROCHOT_event = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->PROCHOT_log = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->crit_temp_status = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->crit_temp_log = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->therm_thresh1_status = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->therm_thresh1_log = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->therm_thresh2_status = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->therm_thresh2_log = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->power_limit_status = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->power_notification_log = (int *) libmsr_malloc(sockets * sizeof(int));
        pts->readout = (int *) libmsr_malloc(sockets * sizeof(int));
        allocate_batch(PKG_THERM_STAT, sockets);
        load_socket_batch(IA32_PACKAGE_THERM_STATUS, pts->raw, PKG_THERM_STAT);
    }

    static void init_temp_target(struct msr_temp_target *tt) {
        static uint64_t sockets = 0;

        sockets = num_sockets();
        tt->raw = (uint64_t **) libmsr_malloc(sockets * sizeof(uint64_t *));
        tt->temp_target = (uint64_t *) libmsr_malloc(sockets * sizeof(uint64_t));
        allocate_batch(TEMP_TARGET, num_sockets());
        load_socket_batch(MSR_TEMPERATURE_TARGET, tt->raw, TEMP_TARGET);
    }

    static void get_temp_target(struct msr_temp_target *s) {
        static uint64_t sockets = 0;
        if(!sockets) sockets = num_sockets();
        read_batch(TEMP_TARGET);
        for (int i = 0; i < sockets; i++) {
            // Minimum temperature at which PROCHOT will be asserted in degree
            // Celsius (probably the TCC Activation Temperature).
            s->temp_target[i] = MASK_VAL(*s->raw[i], 23, 16);
        }
    }

    static void get_pkg_therm_stat(struct pkg_therm_stat *s) {
        uint64_t sockets = num_sockets();

        read_batch(PKG_THERM_STAT);
        for (int i = 0; i < sockets; i++) {
            // Indicates whether the digital thermal sensor high-temp output signal
            // (PROCHOT#) for the pkg currently active. (1=active)
            s->status[i] = MASK_VAL(*s->raw[i], 0, 0);
            
            // Indicates the history of thermal sensor high temp output signal
            // (PROCHOT#) of pkg.
            // (1= pkg PROCHOT# has been asserted since previous reset or last time
            // software cleared bit.
            s->status_log[i] = MASK_VAL(*s->raw[i], 1, 1);
            
            // Indicates whether pkg PROCHOT# is being asserted by another agent on
            // the platform.
            s->PROCHOT_event[i] = MASK_VAL(*s->raw[i], 2, 2);
            
            // Indicates whether pkg PROCHOT# has been asserted by another agent on
            // the platform since the last clearing of the bit by software or
            // reset. (1= has been externally asserted) (write 0 to clear).
            s->PROCHOT_log[i] = MASK_VAL(*s->raw[i], 3, 3);
            
            // Indicates whether pkg crit temp detector output signal is currently
            // active (1=active).
            s->crit_temp_status[i] = MASK_VAL(*s->raw[i], 4, 4);
            
            // Indicates whether pkg crit temp detector output signal been asserted
            // since the last clearing of bit or reset.
            // (1=has been asserted) (set 0 to clear).
            s->crit_temp_log[i] = MASK_VAL(*s->raw[i], 5, 5);
            
            // Indicates whether actual pkg temp is currently higher than or equal
            // to value set in Package Thermal Threshold #1.
            // (0=actual temp lower) (1= actual temp >= PTT#1).
            s->therm_thresh1_status[i] = MASK_VAL(*s->raw[i], 6, 6);
            
            // Indicates whether pkg therm threshold #1 has been reached since last
            // software clear of bit or reset. (1=reached) (clear with 0).
            s->therm_thresh1_log[i] = MASK_VAL(*s->raw[i], 7, 7);
            
            // Same as above (therm_thresh1_stat) except it is for threshold #2.
            s->therm_thresh2_status[i] = MASK_VAL(*s->raw[i], 8, 8);
            
            // Same as above (therm_thresh2_log) except it is for threshold #2
            s->therm_thresh2_log[i] = MASK_VAL(*s->raw[i], 9, 9);
            
            // Indicates pkg power limit forcing 1 or more processors to operate
            // below OS-requested P-state.
            // (Note: pkg power limit violation may be caused by processor cores or
            // by devices residing in the uncore - examine IA32_THERM_STATUS to
            // determine if cause from processor core).
            s->power_limit_status[i] = MASK_VAL(*s->raw[i], 10, 10);
            
            // Indicates any processor from package went below OS-requested P-state
            // or OS-requested clock modulation duty cycle since last clear or
            // RESET.
            s->power_notification_log[i] = MASK_VAL(*s->raw[i], 11, 11);
            
            // Pkg digital temp reading in 1 degree Celsius relative to the pkg TCC
            // activation temp.
            // (0 = Package TCC activation temp)
            // (1 = (PTCC Activation - 1) etc.
            // Note: lower reading actually higher temp
            s->readout[i] = MASK_VAL(*s->raw[i], 22, 16);
        }
    }

    MSRTherm() {
        //uint64_t cores = 0;
        //uint64_t threads = 0;
        //uint64_t sockets = 0;
        
        //if (!sockets) core_config(&cores, &threads, &sockets, NULL);
        //if (init_msr()){
        //    libmsr_error_handler("Unable to initialize libmsr", LIBMSR_ERROR_MSR_INIT, getenv("HOSTNAME"), __FILE__, __LINE__);
        //}

        MSRTherm::init_pkg_therm_stat(&pkg_status_);
        MSRTherm::init_temp_target(&t_target_);
    }

    void measure(){
        MSRTherm::get_temp_target(&t_target_);
        MSRTherm::get_pkg_therm_stat(&pkg_status_);
    }

    int get_temp(int socket) {
        return (t_target_.temp_target[socket] - pkg_status_.readout[socket]);
    }

   ~MSRTherm() {
        //finalize_msr();
    }

    struct pkg_therm_stat pkg_status_; 
    struct msr_temp_target t_target_;  
};

#endif
