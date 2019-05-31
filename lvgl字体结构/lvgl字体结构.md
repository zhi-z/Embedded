# lvgl字体结构



```


lv_font_t lv_font_dejavu_10 = {
    .unicode_first = 32,    /*First Unicode letter in this font*/
    .unicode_last = 126,    /*Last Unicode letter in this font*/
    .h_px = 10,             /*Font height in pixels*/
    .glyph_cnt = 95,            /*Number of glyphs in the font*/
    .glyph_bitmap = lv_font_dejavu_10_glyph_bitmap, /*Bitmap of glyphs*/
    .glyph_dsc = lv_font_dejavu_10_glyph_dsc,       /*Description of glyphs*/
    .unicode_list = NULL,   /*Every character in the font from 'unicode_first' to 'unicode_last'*/
    .get_bitmap = lv_font_get_bitmap_continuous,    /*Function pointer to get glyph's bitmap*/
    .get_width = lv_font_get_width_continuous,  /*Function pointer to get glyph's width*/
};

// 宏定义
void (*const letter_fp) = lv_vletter; 
	
	lv_draw_label(...,style,...)
	letter_fp(&pos, mask, font, letter, color, opa) ==> v_vletter(...,lv_font_t * font_p,...)
		lv_vletter(...,lv_font_t * font_p,...) // 绘制一个字符
			lv_font_get_bitmap(font_p, letter) // 获取字符的bintmap
	
```

