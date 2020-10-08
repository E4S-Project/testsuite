import sys

if len(sys.argv) < 3:
    sys.exit("This script requires specific arguments, run tf_run.sh to provide them automatically")

if sys.argv[1] in ['GPU', 'CPU']:
    print("Running using", sys.argv[1])
else:
    sys.exit("This script requires specific arguments, run tf_run.sh to provide them automatically")

if sys.argv[2] in ['1','2']:
    print("Running using tensorflow V.", sys.argv[2])
else:
    sys.exit("This script requires specific arguments, run tf_run.sh to provide them automatically")


