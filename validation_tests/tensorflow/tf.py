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
import os

#if sys.argv[2]=='NVIDIA':
os.environ["CUDA_VISIBLE_DEVICES"]="1"


## Initialize two constants
#x1 = tf.constant([1,2,3,4])
#x2 = tf.constant([5,6,7,8])
#
## Multiply
#result = tf.multiply(x1, x2)

if sys.argv[3]=='2':

    mnist = tf.keras.datasets.mnist

    (x_train, y_train), (x_test, y_test) = mnist.load_data()
    x_train, x_test = x_train / 255.0, x_test / 255.0

    model = tf.keras.models.Sequential([
      tf.keras.layers.Flatten(input_shape=(28, 28)),
      tf.keras.layers.Dense(128, activation='relu'),
      tf.keras.layers.Dropout(0.2),
      tf.keras.layers.Dense(10)
    ])

    predictions = model(x_train[:1]).numpy()
    predictions

    tf.nn.softmax(predictions).numpy()
    loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
    loss_fn(y_train[:1], predictions).numpy()


    model.compile(optimizer='adam',
                  loss=loss_fn,
                  metrics=['accuracy'])

    model.fit(x_train, y_train, epochs=1)

    result = model.evaluate(x_test,  y_test, verbose=2)


#Results
#========================================

if sys.argv[3]=='1':
    # Initialize Session and run `result`
    with tf.Session() as sess:
      output = sess.run(result)
      print(output)
     
elif sys.argv[3]=='2':
    print(result)

