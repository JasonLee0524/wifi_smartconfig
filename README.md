# smart_config
  wifi一键配网组件源码

## 文件说明：
   |--smart_config       ：组件主目录    
      |--app             ：配网演示代码   
      |--doc             ：配网文档  
      |--hal             ：平台接口代码
         |--common       : 公共文件定义
         |--linux        ：Linux平台实现代码    
      |--sdk             : SDK接口代码     
      |--prj             : 项目工程目录   
         |--linux        : linux项目工程目录   
            |--bin       : 执行文件目录   
            |--build     : 过程文件目录   
            |--lib       : 库文件目录   
               |--inc    : 库文件头文件目录   
      cpack.c            : ieee802.11解析源码    
      cpack.h            : ieee802.11解析头文件  
      utc_smart_config.c : 配网SDK源码  
      utc_smart_config.h : 配网SDK头文件
## 库文件对接方式接口说明
   1、任务调度定时器初始化   
      s32 utc_task_timer_init(void)   
   2、执行任务调度定时器   
      s32 utc_task_timer_process(void)                                                         
   3、打印初始化   
      s32 utc_log_open(void)                                                                 
   4、默认打印等级接口初始化   
      s32 utc_log_set_default_level(void)                                                     
   5、设置模块的打印等级--用于调试    
      s32 utc_log_set_level (u32 module, u32 level)                                          
   6、配网初始化    
      s32 utc_smart_config_open(u8 *p_wifi_name)                                           
   7、获取配网结果   
      s32 utc_smart_config_get_ap(u8 *p_ssid, u16 *p_ssid_len, u8 *p_psk, u16 *p_psk_len)   