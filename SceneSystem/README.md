# Scene Part Program

## 1.修改时间

​	20180118

## 2.修改说明

### 1.PhilipsHue Class

        1. 增加了调节灯光色调的函数，lightCt()和groupCt();用于实现对灯光色度的调节

### 2.DM836II Class

        1. 增加对收到数据的包头进行判断

### 3.Scene Class

1. 场景中的不同时间的颜色不同改为不同时间的色调不同
2. 暂时取消了不同时间不同颜色
3. 在发送的更新CT的后，发送了三次相应的ct数据

