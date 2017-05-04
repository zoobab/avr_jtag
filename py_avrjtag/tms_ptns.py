TMS_IDLE_SEQ = ('\x1f', 6)
TMS_RESET_SEQ = ('\x1f', 5)
TMS_RESET_2_IDLE = ('\x00', 1)

TMS_CAP_DR = ('\x01', 2)
TMS_CAP_IR = ('\x03', 3)
TMS_SHIFT_2_IDLE = ('\x03', 3)
TMS_SHIFT_DR_2_IDLE = TMS_SHIFT_2_IDLE
TMS_SHIFT_IR_2_IDLE = TMS_SHIFT_2_IDLE
TMS_SHIFT_DR_2_CAP_IR = ('\x0f', 5)
TMS_SHIFT_IR_2_CAP_DR = ('\x07', 4)

TMS_SHIFT_DR = ('\x01', 3)
TMS_SHIFT_IR = ('\x03', 4)
TMS_EXIT1_2_IDLE = ('\x01', 2)
TMS_EXIT1_DR_2_IDLE = TMS_EXIT1_2_IDLE
TMS_EXIT1_IR_2_IDLE = TMS_EXIT1_2_IDLE
TMS_EXIT1_2_PAUSE = ('\x00', 1)
TMS_EXIT1_DR_2_PAUSE = TMS_EXIT1_2_PAUSE
TMS_EXIT1_IR_2_PAUSE = TMS_EXIT1_2_PAUSE
TMS_EXIT1_IR_2_SHIFT_DR = ('\x03', 4)
TMS_EXIT1_DR_2_SHIFT_IR = ('\x07', 5)
TMS_EXIT1_2_EXIT2 = ('\x02', 2)
TMS_EXIT1_DR_2_EXIT2_DR = TMS_EXIT1_2_EXIT2
TMS_EXIT1_IR_2_EXIT2_IR = TMS_EXIT1_2_EXIT2
TMS_EXIT1_2_SHIFT = ('\x02', 3)
TMS_EXIT1_DR_2_SHIFT_DR = TMS_EXIT1_2_SHIFT
TMS_EXIT1_IR_2_SHIFT_IR = TMS_EXIT1_2_SHIFT
TMS_EXIT1_DR_2_CAP_IR = ('\x07', 4)
TMS_EXIT1_IR_2_CAP_DR = ('\x03', 3)