import ij.IJ;
import ij.ImagePlus;
import ij.gui.GenericDialog;
import ij.plugin.filter.PlugInFilter;
import ij.process.ImageProcessor;

/**
 * Applies a simple mean low-pass filter on a given image. (Lab 6)
 */
public class MeanUserRadius_ implements PlugInFilter {

	public int setup(String arg, ImagePlus imp) {
		if (arg.equals("about"))
			{showAbout(); return DONE;}
		return DOES_8G+DOES_STACKS+SUPPORTS_MASKING;
	} //setup


	public void run(ImageProcessor ip) {
		byte[] pixels = (byte[])ip.getPixels();
		int width = ip.getWidth();
		int height = ip.getHeight();
		int tgtRadius = 4;

		// TODO

		GenericDialog gd = new GenericDialog("User Input");
		gd.addNumericField("Radius: ", tgtRadius, 0);
		gd.showDialog();
		if (gd.wasCanceled())
			return;

		tgtRadius = (int)gd.getNextNumber();

		int[][] inDataArray = ImageJUtility.convertFrom1DByteArr(pixels, width, height);
		double[][] doubles = ImageJUtility.convertToDoubleArr2D(inDataArray, width, height);
		double[][] meanMask = ConvolutionFilter.getMeanMask(tgtRadius);

		double[][] filtered = ConvolutionFilter.convolveDouble(doubles, width, height, meanMask, tgtRadius);

		ImageJUtility.showNewImage(filtered, width, height, "Mean Filtered");
	} //run

	void showAbout() {
		IJ.showMessage("About Template_...",
			"this is a PluginFilter template\n");
	} //showAbout
} //class Template_

