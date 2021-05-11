import ij.*;
import ij.plugin.filter.PlugInFilter;
import ij.process.*;

public class FilterTemplate_ implements PlugInFilter {  
 
	public int setup(String arg, ImagePlus imp) {
		if (arg.equals("about"))
			{showAbout(); return DONE;}
		return DOES_8G+DOES_STACKS+SUPPORTS_MASKING;
	} //setup

	
	public void run(ImageProcessor ip) {
		byte[] pixels = (byte[])ip.getPixels();
		int width = ip.getWidth();
		int height = ip.getHeight();
	    int[][] inDataArrInt = ImageJUtility.convertFrom1DByteArr(pixels, width, height);
       
        
	} //run

	void showAbout() {
		IJ.showMessage("About Template_...",
			"this is a PluginFilter template\n");
	} //showAbout
	
} //class FilterTemplate_

