struct vbe_info_structure {
    char signature[4] ;	// must be "VESA" to indicate valid VBE support
    uint16_t version;			// VBE version; high byte is major version, low byte is minor version
    uint32_t oem;			// segment:offset pointer to OEM
    uint32_t capabilities;		// bitfield that describes card capabilities
    uint32_t video_modes;		// segment:offset pointer to list of supported video modes
    uint16_t video_memory;		// amount of video memory in 64KB blocks
    uint16_t software_rev;		// software revision
    uint32_t vendor;			// segment:offset to card vendor string
    uint32_t product_name;		// segment:offset to card model name
    uint32_t product_rev;		// segment:offset pointer to product revision
    char reserved[222];		// reserved for future expansion
    char oem_data[256];		// OEM BIOSes store their strings in this area
} __attribute__ ((packed));