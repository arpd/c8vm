#ifndef __ISET_H__
#define __ISET_H__
namespace iset {
    void call_prog(vmstate* state);
    void clear_screen(vmstate* state);
    void ret_routine(vmstate* state);
    void jump(vmstate* state);
    void call_routine(vmstate* state);
    void skip_if_equal(vmstate* state);
    void skip_if_not_equal(vmstate* state);
    void skip_if_equal_regs(vmstate* state);
    void set_reg(vmstate* state);
    void add_reg(vmstate* state);
    void set_regx_regy(vmstate* state);
    void set_regx_or_regy(vmstate* state);
    void set_regx_and_regy(vmstate* state);
    void set_regx_xor_regy(vmstate* state);
    void set_regx_add_regy(vmstate* state);
    void set_regx_sub_regy(vmstate* state);
    void set_regx_rshift(vmstate* state);
    void set_regx_regy_sub_regx(vmstate* state);
    void set_regx_lshift(vmstate* state);
    void skip_if_not_equal_regs(vmstate* state);
    void set_index(vmstate* state);
    void jump_offset(vmstate* state);
    void set_reg_rand_masked(vmstate* state);
    void draw_sprite(vmstate* state);
    void skip_if_key_pressed(vmstate* state);
    void skip_if_key_not_pressed(vmstate* state);
    void set_reg_delay(vmstate* state);
    void wait_key_press_store(vmstate* state);
    void set_delay_regx(vmstate* state);
    void set_sound_regx(vmstate* state);
    void add_regx_to_index(vmstate* state);
    void get_sprite_regx(vmstate* state);
    void split_decimal(vmstate* state);
    void dump_regs_to_regx(vmstate* state);
    void slurp_regs_to_regx(vmstate* state);
};
#endif
