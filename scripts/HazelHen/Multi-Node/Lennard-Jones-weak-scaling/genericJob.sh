#!/bin/bash
#PBS -N mardyn
#PBS -l nodes=NNARG:ppn=PPNARG
#PBS -l walltime=TLARG
#PBS -o OFARG
#PBS -e EFARG
#PBS -m abe
#PBS -M seckler@in.tum.de
# Change to the directory that the job was submitted from
cd $PBS_O_WORKDIR
# Launch the parallel job to the allocated compute nodes
#printenv | grep PBS
HYPERTHREADING=HYPARG
npes=$PBS_NP
ppn=$PBS_NUM_PPN
omps=$((24*$HYPERTHREADING/$ppn))
nodes=$(($npes/$ppn))
echo ""
echo "running with $ppn processes per node ($omps omp threads per process) on $nodes nodes."
echo "using $HYPERTHREADING threads per core."
echo ""
export OMP_NUM_THREADS=$omps

export MPICH_MAX_SHORT_MSG_SIZE=4096
export MPICH_UNEX_BUFFER_SIZE=8M

module list
#aprun -n $npes -N $ppn -d $OMP_NUM_THREADS -cc depth a.out
executable="EXARG"
#aprun -n $npes -N $ppn -d $OMP_NUM_THREADS -cc depth -j $HYPERTHREADING ../$executable IFARG 50 --final-checkpoint=0
aprun -n $npes -N $ppn -d $OMP_NUM_THREADS -cc cpu -j $HYPERTHREADING ../../exec/$executable IFARG --steps=11 --final-checkpoint=0
#aprun -n 48 -N 24 ./exec/MarDyn.PAR_DEBUG_AOS_4251 inp/simple-lj.cfg 10 --final-checkpoint=0