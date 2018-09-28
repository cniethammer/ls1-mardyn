#!/bin/bash
#@ job_type = parallel
###@ ll_res_id=srv03-ib.56.r
###@ energy_policy_tag = lu78toq.tag_min_t2s
###@ group = vip
###@ set_frequency = 2.3
#@ initialdir=/home/hpc/pr48te/di56giq5/svn/WR_2017/SuperMUC-Phase1/2017-10-24/1clj-single-rc35/hybrid
#@ job_name = mar-h-64-1-sp-35
#@ class = general
#@ node_usage = not_shared
#@ wall_clock_limit = 05:00:00
#@ network.MPI = sn_all,not_shared,us
#@ notification = always
#@ notify_user = tchipev@in.tum.de
#@ output = output-of-jobs-64-nodes/LLRUN.out.64.23Ghz.$(jobid)
#@ error =  output-of-jobs-64-nodes/LLRUN.err.64.23Ghz.$(jobid)
#@ node = 64
#@ island_count=1,1
#@ total_tasks = 64
#@ queue



### @ Other people using this script - change the Email address line! notify_user = xx@in.tum.de

NumProcs=64
executableName=MarDyn_5713.PAR_RELEASE_AVX-gcc-7-ibmmpi-RMM-SINGLE


#-------------------------------------
#Settings for POE
#-------------------------------------
. /etc/profile
. /etc/profile.d/modules.sh

module unload mpi.ibm
module load mpi.ibm/1.4_gcc


export MP_SINGLE_THREAD=no
export OMP_NUM_THREADS=32

export MP_TASK_AFFINITY=cpu:$OMP_NUM_THREADS

export MP_SINGLE_THREAD=no
export MP_USE_BULK_XFER=no

# for loop over system sizes
for ((iRepe=0; iRepe <= 4; iRepe +=1)) ; 
do
	echo "repetition $iRepe"
	for ((iSize=4096; iSize >= 1; iSize /= 2)) ;
	do
		echo "system size: $iSize"
		#for loop over schemes
		schemes="slice c08"
		for iScheme in $schemes ;
		do
			echo "scheme: $iScheme"
			inputFileName=64-nodes-$iSize-$iScheme.xml
			outputFileName="output-of-jobs-64-nodes/out-$NumProcs-$iSize-$iScheme-$iRepe.txt"
			mpiexec -n $NumProcs ../../$executableName $inputFileName --steps 11 --final-checkpoint=0 >$outputFileName
		done
	done
done
