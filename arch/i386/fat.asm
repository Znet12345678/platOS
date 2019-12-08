section .data
;fat 16 is the only supported fat version rn
fat_BS:
bootjmp:times 3 db 0
oem_name:times 8 db 0
bytes_per_sector:dw 0
sectors_per_cluster:db 0
reserved_sector_count:dw 0
table_count:db 0
root_entry_count:dw 0
total_sectors_16:dw 0
media_type:db 0
table_size_16:dw 0
sectors_per_track:dw 0
head_side_count:dw 0
hidden_sector_count:dd 0
total_sectors_32:dd 0
fBS_End:
ext_BS:
bios_drive_num:db 0
reserved1:db 0
boot_signature:db 0
volume_id:dd 0
volume_label:times 11 db 0
fat_type_label:times 8 db 0
ext_BS_End:
fat_BS_End:
section .text
