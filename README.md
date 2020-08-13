# TimeWheel
>## OverAll<br>
>>This is a time Wheel with C++. And providing c(in the future) and c++ api.<br>
>>Time wheel is an object including some wheels with many frame, which can run functions in any frame.Every wheel has different frames, what's more, the speed of every wheel is totally different from the others.  <br>
>>The lower wheel runs faster than upper wheel, and the wheel will change(in other word,it jumps to next frame) when the lower wheel makes a trun except the lowest wheel.<br>
>>The lowest wheel jumps to next frame when the system singal reached.
>## OverAll<br>
>> **Init**:                   Init the time wheel, call this function before use this time wheel.<br>
>> **Start**:                  Let the time wheel start running. Some functions may be different after call this function.<br>
>> **Stop**:                   Let the time wheel stop running.<br>
>> **AddFunction**:            Add funtion be called in the feature. It will be nonsynchronous when the time wheel running.<br>
> **DeleteFunction**:         Remove the function in time wheel(This function will be provieded in the future).<br>
>> **DeleteOneFrameFunction**: Reomve the function with the same frame you select.<br>
>## Feature<br>
>> ### Add<br>
>>* A function which can delete function you select with a ID provide when the function added to time wheel.<br>
>>* A safer Multi-threading process.<br>
>> ### Modify<br>
>>* Return ID when the function add to the time wheel.<br>
>## Contact me
>> + E-mail: symraingit@163.com <br>
>> + Or you can also contact me in github.
