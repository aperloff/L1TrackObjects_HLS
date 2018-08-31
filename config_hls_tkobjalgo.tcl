# set the board being used (Options MP7, CTP7, VCU118)
set l1board VCU118

# define which function to use
set l1tkobjAlgo "tkobjalgo"

# vertexing implementation
set l1tkobjTopFunc mp7wrapped_${l1tkobjAlgo}

# reference implementation
set l1tkobjRefFunc ${l1tkobjAlgo}_ref

# set to zero to turn off C validation (runs but does not check against the reference implementation)
set l1tkobjValidate 1

# set the debug level
set l1tkobjDebug 0

# set to zero to turn off track quality cuts
set l1tkQualityCuts 0

# 1 = verbose, 0 = less verbose
set l1tkobjVerbose 0

# version of the IP Core output
set l1tkobjIPVersion 1.0
