import sys

#Call Check
#========================================

if len(sys.argv) < 4:
    sys.exit("This script requires specific arguments, run tf_run.sh to provide them automatically")

if sys.argv[1] in ['GPU', 'CPU']:
    if sys.argv[2] in ['NVIDIA', 'AMD']:
        print("Running using", sys.argv[2], sys.argv[1])
    elif sys.argv[2] in ['NA']:
        print("Running using", sys.argv[1])
else:
    sys.exit("This script requires specific arguments, run tf_run.sh to provide them automatically")

if sys.argv[3] in ['1','2']:
    print("Running using tensorflow V.", sys.argv[3])
else:
    sys.exit("This script requires specific arguments, run tf_run.sh to provide them automatically")


#Code
#========================================

# Import `tensorflow`
import tensorflow as tf

print(sys.argv[3])
if sys.argv[3]=='1':
    print("T!")

    # Initialize two constants
    x1 = tf.constant([1,2,3,4])
    x2 = tf.constant([5,6,7,8])

    # Multiply
    result = tf.multiply(x1, x2)

    # Initialize Session and run `result`
    with tf.Session() as sess:
      output = sess.run(result)
      print(output)
     
if sys.argv[3]=='2':
    print(sys.argv[3])
    print("test")

    # Initialize two constants
    x1 = tf.constant([1,2,3,4])
    x2 = tf.constant([5,6,7,8])

    # Multiply
    result = tf.multiply(x1, x2)
    print(result)

