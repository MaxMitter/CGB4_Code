import ij.IJ;
import ij.ImagePlus;
import ij.plugin.filter.PlugInFilter;
import ij.process.ImageProcessor;

/**
 * Plugin that inverts a given greyscale image. (Lab 5)
 */
public class Invert_ implements PlugInFilter {

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

        int[] invertTf = ImageTransformationFilter.getInversionTF(255);
        int[][] transformedImage = ImageTransformationFilter.getTransformedImage(inDataArrInt, width, height, invertTf);

        ImageJUtility.showNewImage(transformedImage, width, height, "Inverted Image");
	} //run

	void showAbout() {
		IJ.showMessage("About Template_...",
			"this is a PluginFilter template\n");
	} //showAbout

} //class Invert_

