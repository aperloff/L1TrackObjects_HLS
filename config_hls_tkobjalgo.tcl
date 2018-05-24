# set the board being used
set l1board CTP7

# define which function to use
set l1tkobjAlgo "tkobjalgo"

# vertexing implementation
set l1tkobjTopFunc mp7wrapped_${l1tkobjAlgo}

# reference implementation
set l1tkobjRefFunc ${l1tkobjAlgo}_ref

# set to zero to turn off C validation (runs but does not check against the reference implementation)
set l1tkobjValidate 1

# set to zero to turn off track quality cuts
set l1tkQualityCuts 0

# version of the IP Core output
set l1tkobjIPVersion 1.0
