#!/bin/bash
#@ energy_policy_tag = tchipev_lammps
#@ minimize_time_to_solution = yes
##
## optional: energy policy tags
#
# DO NOT USE environment = COPY_ALL
#@ job_type = MPICH
#@ class = test
#@ node = 1
#@ island_count=1,1
#@ total_tasks=2
#@ wall_clock_limit = 00:30:00
#@ job_name = lammps-first
#@ network.MPI = sn_all,not_shared,us
#@ initialdir = $(home)/svn/WR_2017/SuperMUC-Phase1/lammps-comparison/lammps-their-lj/
#@ output = output-of-jobs/job$(jobid).out
#@ error = output-of-jobs/job$(jobid).err
#@ notification=always
#@ notify_user=tchipev@in.tum.de
#@ queue
. /etc/profile
. /etc/profile.d/modules.sh
#setup of environment
module unload mpi.ibm
module load mpi.intel
module switch intel intel/18.0
module switch mkl mkl/2018
module load tbb/2018

export OMP_NUM_THREADS=16

#0000000 from USER-INTEL/TESTS/README
export I_MPI_PIN_DOMAIN=omp
export I_MPI_FABRICS=shm		# For single node


for i in {0..3} ;
do
	mpiexec -n 2 ../lmp_intel_cpu_intelmpi -in in.intel.lj.newton -log none > output-of-jobs/newton-out-lammps-2-16-$i.txt
done
