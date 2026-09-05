/**
 * @file bsp_buzzer.c
 * @brief Buzzer control encapsulates implementation.
 * @author Gao Xing
 * @date 2025/2/9
 * @version 1.0
 */

#include "bsp_buzzer.h"

/**
 * @brief 启动蜂鸣器
 */
void startBuzzer(void)
{
    HAL_TIM_Base_Start(&htim4);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
}

/**
 * @brief 停止蜂鸣器
 */
void stopBuzzer(void)
{
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, 0);
}

/**
 * @brief 设置蜂鸣器频率
 * @param frequency 频率值
 */
static void setBuzzerFrequency(float frequency)
{
    uint32_t psc = 319; // 预分频值
    uint32_t arr = (uint32_t)((TIM_CLOCK_FREQ / ((psc + 1) * frequency)) - 1);

    __HAL_TIM_PRESCALER(&htim4, psc);
    __HAL_TIM_SET_AUTORELOAD(&htim4, arr);
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, arr / 2); // 设置占空比为 50%
}

/**
 * @brief 播放指定音符
 * @param note 音符类型
 */
void playNote(Note_Type note)
{
    float frequency;
    switch (note)
    {
    case NOTE_DO:
        frequency = 523.25; // Do (C5)
        break;
    case NOTE_RE:
        frequency = 587.33; // Re (D5)
        break;
    case NOTE_MI:
        frequency = 659.25; // Mi (E5)
        break;
    case NOTE_FA:
        frequency = 698.46; // Fa (F5)
        break;
    case NOTE_SO:
        frequency = 783.99; // So (G5)
        break;
    case NOTE_LA:
        frequency = 880.00; // La (A5)
        break;
    case NOTE_SI:
        frequency = 987.77; // Si (B5)
        break;
    default:
			  frequency=0;
        return; // 无效的音符
    }
    setBuzzerFrequency(frequency);
}
//------------------------------------------------------------------------------
/*以下为音乐播放器实现*/

/** @brief 全局音乐播放器实例 */
Music_Player_s music_player = {0};

/**
 * @brief 预设的音乐序列
 * @note 包含音符和对应的持续时间(ms)
 */
static const Music_Note_s its_my_go_notes[] = {

 {NOTE_MI, 200}, {NOTE_MI, 200}, {NOTE_RE, 200}, {NOTE_MI, 400},
  {NOTE_MI, 300}, {NOTE_RE, 200}, {NOTE_MI, 400}, {NOTE_SO, 400},{NOTE, 200},
    
 {NOTE_MI, 200}, {NOTE_MI, 200}, {NOTE_RE, 200}, {NOTE_MI,400},
  {NOTE_RE, 200}, {NOTE_MI, 200},  {NOTE_SO, 400},{NOTE_DO, 200},{NOTE, 200},
    
  {NOTE_MI, 400}, {NOTE_MI, 400}, {NOTE_RE, 400}, {NOTE_MI, 400},
   {NOTE_RE, 400}, {NOTE_RE, 200}, {NOTE_DO, 400},{NOTE_RE, 200},

	};


/**
 * @brief 停止音乐播放
 * @param player 音乐播放器指针
 * @details 停止播放并关闭蜂鸣器输出
 */
void stopMusicPlay(Music_Player_s *player)
{
    player->is_playing = false;
    stopBuzzer();
}

/**
 * @brief 更新音乐播放器状态
 * @param player 音乐播放器指针
 * @details 检查当前音符是否播放完成，需要在主循环中定期调用
 * @note 实现非阻塞式音乐播放的核心函数
 */

    uint32_t current_time;
    uint32_t elapsed_time;
void updateMusicPlayer(Music_Player_s *player)
{
    if (!player->is_playing)
        return;

     current_time = HAL_GetTick();
     elapsed_time = current_time - player->note_start_time;

    // 检查当前音符是否播放完成
    if (elapsed_time >= player->notes[player->current_note].duration)
    {
        player->current_note++;

        // 检查是否播放完所有音符
        if (player->current_note >= player->total_notes)
        {
            stopMusicPlay(player);
            return;
        }

        // 播放下一个音符
        player->note_start_time = current_time;
        playNote(player->notes[player->current_note].note);
    }
}

/**
 * @brief 开始播放音乐序列
 * @param player 音乐播放器指针
 * @param notes 音符序列
 * @param total_notes 音符数量
 * @details 初始化播放器状态并开始播放第一个音符
 */
void startMusicPlay(Music_Player_s *player, const Music_Note_s *notes, uint16_t total_notes)
{
    player->notes = notes;
    player->total_notes = total_notes;
    player->current_note = 0;
    player->note_start_time = HAL_GetTick();
    player->is_playing = true;
    startBuzzer();
    playNote(notes[0].note);
}

/**
 * @brief 你愿意和我组一辈子乐队吗
*         Yes I Do.
 */
void ItsMyGo(void)
{
    startMusicPlay(&music_player, its_my_go_notes, sizeof(its_my_go_notes) / sizeof(Music_Note_s));
}
