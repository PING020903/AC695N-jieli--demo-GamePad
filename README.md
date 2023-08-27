# AC695N-demo_xbox360_WindowsPC_only
基于杰理AC695N系列，AC6951C的游戏手柄控制器   
将usb通讯抓包，模仿Xbox360对WindowsPC的通信   
测试在WindowsPC中有反应能用，但没做Xbox主机适配
# 该demo project本人是用code::blocks打开.cbp文件，按齿轮形状的按钮( build )，即便可以进行编译( 前提是你的电脑需要事先安装GUN编译 )   
   
# 按下 build 后若你的电脑有连接开发板，且开发板处于烧录模式，即可自动下载到 flash 中    
若要添加功能，请在app_dongle.c中app_start添加你的任务初始化函数，可参照当前代码中添加任务初始化的形式。   
若要添加.c文件进行编译，请修改你要添加.c文件的文件夹中的makefile    
[![image](https://github.com/PING020903/AC695N-demo-xbox360-WindowsPC_only/assets/88314322/25880817-038e-4739-8898-3da2a77fad6b)]   
如图中被选中的行，按照该格式，在该行下方添加自己的.c文件，编译时即可包含你自己所创建的.c文件    
# 设备描述符与配置描述符在 descriptor.c, 接口描述符在 uac1.c, 配置描述符与接口描述符的拼接在 uar1.c 中，该配置描述符中的配置总长度会经过SDK中自行算出，初值赋值并不重要  

# 更多请阅读该路径下的详细代码逻辑描述
> /sdk/Code_Logic_Description_CN.md
