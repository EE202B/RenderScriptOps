# RenderScriptOps
RenderScript support ops for TensorFlow

## Targeting Ops:
Two most energy and time consumption ops on Android device.
* matmul_op
* conv_op

## API
Please check out the document at page 15 of [TensorFlow-RenderScript Doc](https://github.com/EE202B/tensorflow/blob/RenderScript/tensorflow/contrib/android_renderscript_ops/doc/Tensorflow-RenderScript.pdf).

## Notice
* The test data was fetched from TensorFlow runtime data.
* NDK version>=r14
* No need to worry to the two error messages of matmul uint8_t test, I just too lazy to rewrite the correct reference output computation.

## Integration with TensorFlow
An integration with TF can be see here: [TensorFlow-RenderScript](https://github.com/EE202B/tensorflow/tree/RenderScript/tensorflow/contrib/android_renderscript_ops).
