/**
 * @brief 通信模式控制函数
 * @details 根据遥控器状态和控制指令，设置底盘、云台和射击系统的工作模式
 * @note 支持DT7遥控器和VT13遥控器两种控制方式
 */
void Mode_Com()
{
    /* 处理DT7遥控器控制状态 */
    if (Comm_Info.Remote_State == DT7_Ctrl)
    {
        /* 遥控器未丢失时 */
        if (remote_ctrl.rc_lost == 0)
        {
            /* 遥控器控制模式 */
            if (Comm_Info.Control_Mode == Remote_Control)
            {
                /* 默认设置为连发模式 */
                Comm_Info.Shoot_Mode = Shoot_Repeat;

                /* 遥控器未丢失时处理遥控器开关信号 */
                if (remote_ctrl.rc_lost != true)
                {
                    /* 根据遥控器右侧开关状态(s[1])进行模式切换 */
                    switch (remote_ctrl.rc.s[1])
                    {
                    case 1: /* 右侧开关处于上档位 */
                        /* 根据遥控器左侧开关状态(s[0])进一步细分模式 */
                        switch (remote_ctrl.rc.s[0])
                        {
                        case 1: /* 左侧开关处于上档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_Follow; /* 底盘跟随模式 */
                            Comm_Info.Gimbal_Mode = Gimbal_ON;       /* 云台开启 */
                            Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        case 3: /* 左侧开关处于下档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_Tune;   /* 底盘调试模式 */
                            Comm_Info.Gimbal_Mode = Gimbal_ON;       /* 云台开启 */
                            Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        case 2: /* 左侧开关处于中间档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_OFF;    /* 底盘关闭 */
                            Comm_Info.Gimbal_Mode = Gimbal_OFF;      /* 云台关闭 */
                            Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        }
                        break;

                    case 3: /* 右侧开关处于下档位 */
                        /* 根据遥控器左侧开关状态(s[0])进一步细分模式 */
                        switch (remote_ctrl.rc.s[0])
                        {
                        case 1: /* 左侧开关处于上档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_Follow; /* 底盘跟随模式 */
                            Comm_Info.Gimbal_Mode = Gimbal_ON;       /* 云台开启 */
                            Comm_Info.Shoot_Mode = Shoot_Repeat;     /* 射击连发模式 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        case 3: /* 左侧开关处于下档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_Spin;   /* 底盘旋转模式 */
                            Comm_Info.Gimbal_Mode = Gimbal_ON;       /* 云台开启 */
                            Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        case 2: /* 左侧开关处于中间档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_OFF;    /* 底盘关闭 */
                            Comm_Info.Gimbal_Mode = Gimbal_ON;       /* 云台开启 */
                            Comm_Info.Shoot_Mode = Shoot_Single;     /* 射击单发模式 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        }
                        break;

                    case 2: /* 右侧开关处于中间档位 */
                        /* 根据遥控器左侧开关状态(s[0])进一步细分模式 */
                        switch (remote_ctrl.rc.s[0])
                        {
                        case 1: /* 左侧开关处于上档位 */
                        {
                            Comm_Info.Control_Mode = KeyBoard_Control; /* 切换到键盘控制模式 */
                        }
                        break;
                        case 3: /* 左侧开关处于下档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_OFF;    /* 底盘关闭 */
                            Comm_Info.Gimbal_Mode = Gimbal_OFF;      /* 云台关闭 */
                            Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                            /* 如果有通道4输入，则切换到VT13遥控器控制 */
                            if (remote_ctrl.rc.ch[4] != 0)
                                Comm_Info.Remote_State = VT13_Ctrl;
                        }
                        break;
                        case 2: /* 左侧开关处于中间档位 */
                        {
                            Comm_Info.Chassis_Mode = Chassis_OFF;    /* 底盘关闭 */
                            Comm_Info.Gimbal_Mode = Gimbal_OFF;      /* 云台关闭 */
                            Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                            Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                        }
                        break;
                        }
                        break;
                    default: /* 开关状态异常时的安全模式 */
                    {
                        Comm_Info.Chassis_Mode = Chassis_OFF;    /* 底盘关闭 */
                        Comm_Info.Gimbal_Mode = Gimbal_OFF;      /* 云台关闭 */
                        Comm_Info.Shoot_Mode = Shoot_OFF;        /* 射击关闭 */
                        Comm_Info.Control_Mode = Remote_Control; /* 保持遥控器控制 */
                    }
                    break;
                    }
                }
                else
                {
                    /* 遥控器丢失时的安全模式 */
                    Comm_Info.Chassis_Mode = Chassis_OFF; /* 底盘关闭 */
                    Comm_Info.Gimbal_Mode = Gimbal_OFF;   /* 云台关闭 */
                    Comm_Info.Shoot_Mode = Shoot_OFF;     /* 射击关闭 */
                }
            }

            /* 键盘控制模式 */
            else if (Comm_Info.Control_Mode == KeyBoard_Control)
            {
                /* 如果遥控器开关状态改变，则切换回遥控器控制 */
                if ((remote_ctrl.rc.s[1] != 2) || (remote_ctrl.rc.s[0] != 1))
                    Comm_Info.Control_Mode = Remote_Control;

                /* 底盘丢失时设置为跟随模式 */
                if (Comm_Info.Chassis_Lost == 1)
                    Comm_Info.Chassis_Mode = Chassis_Follow;
                /* 键盘C键按下时设置为底盘调试模式 */
                if (KeyBoard_Info.key.set.C == 1)
                    Comm_Info.Chassis_Mode = Chassis_Tune;

                /* SHIFT键切换底盘跟随/旋转模式 */
                static bool SHIFT_Flag = 0; /* SHIFT键状态标志 */
                if (KeyBoard_Info.key.set.SHIFT == 1 && SHIFT_Flag == 0)
                {
                    if (Comm_Info.Chassis_Mode == Chassis_Follow)
                    {
                        Comm_Info.Chassis_Mode = Chassis_Spin; /* 切换到旋转模式 */
                    }
                    else if (Comm_Info.Chassis_Mode != Chassis_Follow)
                    {
                        Comm_Info.Chassis_Mode = Chassis_Follow; /* 切换到跟随模式 */
                    }
                    SHIFT_Flag = 1; /* 标记SHIFT键已按下 */
                }
                else if (KeyBoard_Info.key.set.SHIFT == 0)
                    SHIFT_Flag = 0; /* 标记SHIFT键已释放 */
            }
        }
        else
        {
            /* 遥控器丢失时的安全模式 */
            Comm_Info.Chassis_Mode = Chassis_OFF; /* 底盘关闭 */
            Comm_Info.Gimbal_Mode = Gimbal_OFF;   /* 云台关闭 */
            Comm_Info.Shoot_Mode = Shoot_OFF;     /* 射击关闭 */
        }
    }
    /* 处理VT13遥控器控制状态 */
    else if (Comm_Info.Remote_State == VT13_Ctrl)
    {
        /* 根据VT13遥控器开关状态设置控制模式 */
        if (VT13_Info.RC.Switch == 1)
            Comm_Info.Control_Mode = Remote_Control; /* 遥控器控制 */
        else if (VT13_Info.RC.Switch == 2)
            Comm_Info.Control_Mode = KeyBoard_Control; /* 键盘控制 */
        else
        {
            /* 开关状态异常时的安全模式 */
            Comm_Info.Chassis_Mode = Chassis_OFF; /* 底盘关闭 */
            Comm_Info.Gimbal_Mode = Gimbal_OFF;   /* 云台关闭 */
            Comm_Info.Shoot_Mode = Shoot_OFF;     /* 射击关闭 */
            /* STOP键按下时切换回DT7遥控器控制 */
            if (VT13_Info.RC.Stop == 1)
                Comm_Info.Remote_State = DT7_Ctrl;
        }

        /* VT13遥控器控制模式 */
        if (Comm_Info.Control_Mode == Remote_Control)
        {
            /* 右键控制云台模式切换 */
            static bool Right_Flag = 0; /* 右键状态标志 */
            if (VT13_Info.RC.Right == 1 && Right_Flag == 0)
            {
                if (Comm_Info.Gimbal_Mode == Gimbal_ON)
                {
                    Comm_Info.Gimbal_Mode = Gimbal_AotoAim; /* 切换到自动瞄准模式 */
                }
                else if (Comm_Info.Gimbal_Mode != Gimbal_ON)
                {
                    Comm_Info.Gimbal_Mode = Gimbal_ON; /* 切换到云台开启模式 */
                }
                Right_Flag = 1; /* 标记右键已按下 */
            }
            else if (VT13_Info.RC.Right == 0)
                Right_Flag = 0; /* 标记右键已释放 */

            /* 左键控制射击模式切换 */
            static bool Left_Flag = 0; /* 左键状态标志 */
            if (VT13_Info.RC.Left == 1 && VT13_Info.RC.Stop == 1)
                Comm_Info.Shoot_Mode = Shoot_OFF; /* STOP键和左键同时按下时关闭射击 */
            if (VT13_Info.RC.Left == 1 && Left_Flag == 0)
            {
                if (Comm_Info.Shoot_Mode == Shoot_Repeat)
                {
                    Comm_Info.Shoot_Mode = Shoot_Single; /* 切换到单发模式 */
                }
                else if (Comm_Info.Shoot_Mode != Shoot_Repeat)
                {
                    Comm_Info.Shoot_Mode = Shoot_Repeat; /* 切换到连发模式 */
                }
                Left_Flag = 1; /* 标记左键已按下 */
            }
            else if (VT13_Info.RC.Left == 0)
                Left_Flag = 0; /* 标记左键已释放 */

            /* 扳机控制底盘模式切换 */
            static bool Trigger_Flag; /* 扳机状态标志 */
            if (VT13_Info.RC.Stop == 1 && VT13_Info.RC.Trigger == 1)
                Comm_Info.Chassis_Mode = Chassis_Tune; /* STOP键和扳机同时按下时进入调试模式 */
            if (VT13_Info.RC.Trigger == 1 && Trigger_Flag == 0)
            {
                if (Comm_Info.Chassis_Mode == Chassis_Follow)
                {
                    Comm_Info.Chassis_Mode = Chassis_Spin; /* 切换到旋转模式 */
                }
                else if (Comm_Info.Chassis_Mode != Chassis_Follow)
                {
                    Comm_Info.Chassis_Mode = Chassis_Follow; /* 切换到跟随模式 */
                }
                Trigger_Flag = 1; /* 标记扳机已按下 */
            }
            else if (VT13_Info.RC.Trigger == 0)
                Trigger_Flag = 0; /* 标记扳机已释放 */
        }

        /* 键盘控制模式 */
        else if (Comm_Info.Control_Mode == KeyBoard_Control)
        {
            /* 鼠标右键控制云台自动瞄准 */
            if (KeyBoard_Info.mouse.press_r == 1)
            {
                Comm_Info.Gimbal_Mode = Gimbal_AotoAim; /* 自动瞄准模式 */
            }
            else
            {
                Comm_Info.Gimbal_Mode = Gimbal_ON; /* 云台开启模式 */
            }

            /* 底盘丢失时设置为跟随模式 */
            if (Comm_Info.Chassis_Lost == 1)
                Comm_Info.Chassis_Mode = Chassis_Follow;
            /* 键盘C键按下时设置为底盘调试模式 */
            if (KeyBoard_Info.key.set.C == 1)
                Comm_Info.Chassis_Mode = Chassis_Tune;

            /* SHIFT键切换底盘跟随/旋转模式 */
            static bool SHIFT_Flag = 0; /* SHIFT键状态标志 */
            if (KeyBoard_Info.key.set.SHIFT == 1 && SHIFT_Flag == 0)
            {
                if (Comm_Info.Chassis_Mode == Chassis_Follow)
                {
                    Comm_Info.Chassis_Mode = Chassis_Spin; /* 切换到旋转模式 */
                }
                else if (Comm_Info.Chassis_Mode != Chassis_Follow)
                {
                    Comm_Info.Chassis_Mode = Chassis_Follow; /* 切换到跟随模式 */
                }
                SHIFT_Flag = 1; /* 标记SHIFT键已按下 */
            }
            else if (KeyBoard_Info.key.set.SHIFT == 0)
                SHIFT_Flag = 0; /* 标记SHIFT键已释放 */

            /* V键长按触发系统复位 */
            static int16_t V_Flag = 0;
            if (KeyBoard_Info.key.set.V == 1 && V_Flag <= 1000)
            {
                V_Flag++;               /* 计数V键按下时间 */
                HAL_NVIC_SystemReset(); /* 触发系统复位 */
            }
            else
                V_Flag = 0; /* 重置计数 */
        }
    }
    else
    {
        /* 其他遥控器状态时的安全模式 */
        Comm_Info.Chassis_Mode = Chassis_OFF; /* 底盘关闭 */
        Comm_Info.Gimbal_Mode = Gimbal_OFF;   /* 云台关闭 */
        Comm_Info.Shoot_Mode = Shoot_OFF;     /* 射击关闭 */
    }
}