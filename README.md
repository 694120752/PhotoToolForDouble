# PhotoToolForDouble

##
主线：
接受图片打上水印
支线：
检索关键字信息返回

### 结构分层
1.Server端
    - socket接受所有请求分发
    TODO：
    - 接受9张图保存至本地
    - Opencv 识别人脸圈出
    - 寻找指定位置添加水印
    
2.Client端
    - 发送图片接受展示
