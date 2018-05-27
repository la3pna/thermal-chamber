# thermal-chamber
Thermal chamber from freezer, with custom controller and PC program.

SCPI Commands:
   *  *IDN?         -> identify
   *  *RST          -> reset to local
   *  :TRIGger
   *    :SINGle    -> trig_single
   *    :CONTinous   -> trig_cont
   *  :MEASure
   *    :TEMPerature?   -> get_alltemp
   *    :TEMPerature1?   -> get_temp1
   *    :TEMPerature2?   -> get_temp2
   *    :TEMPerature3?   -> get_temp3
   *    :TEMPerature4?   -> get_temp4
   *    :INTernal1?      -> get_int1
   *    :INTernal2?      -> get_int2
   *    :INTernal3?      -> get_int3
   *    :INTernal4?      -> get_int4
   *  :SYStem
   *    :VOLTage?   -> get_voltage
   *    :ERRor?     -> get_err
   *    :PRINT      -> display_print
   *  :UNIT
   *    :TEMPerature -> set_temperature_unit (C/CEL/F/FAR/K/KEL)
   *    :TEMPerature? -> get_temprature unit (C/CEL/F/FAR/K/KEL)
