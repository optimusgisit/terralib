inputImage=READ_IMAGE( "cbers2b_rgb342_crop.tif" )

labelImage = TERRALIB_RP_SEGMENTER_RG( inputImage, 20, 10 )

WRITE_IMAGE, "terralid_idl_rp_segmenter.tif", "TIFF", labelImage

exit
