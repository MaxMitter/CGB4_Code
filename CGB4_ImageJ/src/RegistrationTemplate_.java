import ij.IJ;
import ij.ImagePlus;
import ij.plugin.filter.PlugInFilter;
import ij.process.ImageProcessor;

/**
 * Simple implementation of a registration using nearest neighbor interpolation and sum of squared error distance
 * metric. (Lab 8)
 */
public class RegistrationTemplate_ implements PlugInFilter {

    public int setup(String arg, ImagePlus imp) {
        if (arg.equals("about")) {
            showAbout();
            return DONE;
        }
        return DOES_8G + DOES_STACKS + SUPPORTS_MASKING;
    } //setup

    /**
     * @param xIdx   double x-image coordinate
     * @param yIdx   double y-image coordinate
     * @param width  the width of the input image
     * @param height the height of the image image
     * @param img    input image
     * @return the interpolated value for x and y.
     */
    public int getNNinterpolatedValue(double xIdx, double yIdx, int width, int height, int[][] img) {
        //just round the coordinates
        // TODO
        return -1;
    }

    /**
     * @param inImg    input image
     * @param width    the width of the input image
     * @param height   the height of the input image
     * @param transX   translation in x-direction
     * @param transY   translation in y-direction
     * @param rotAngle rotation angle in degrees
     * @return transformed image
     */
    public int[][] transformImg(int[][] inImg, int width, int height, double transX, double transY, double rotAngle) {
        int[][] resultImg = new int[width][height];

        // TODO

        return resultImg;
    }

    /**
     * @param refImg  - static reference image
     * @param testImg - moving image getting transformed
     * @param width   the width of the input image
     * @param height  the height of the input image
     * @return error metric
     */
    public double getImgDiffSSE(int[][] refImg, int[][] testImg, int width, int height) {
        double totalError = 0.0;

        // TODO

        return totalError;
    }

    /**
     * @param refImg  - static reference image
     * @param testImg - moving image getting transformed
     * @param width   the width of the input image
     * @param height  the height of the input image
     * @return difference image
     */
    public double[][] getDiffImg(double[][] refImg, double[][] testImg, int width, int height) {
        double[][] res = new double[width][height];

        // TODO

        return res;
    }

    /**
     * method for automated registration utilizing 11x11x11 permutations for the transformation parameters
     *
     * @param refImg  The reference image
     * @param testImg the image to test with.
     * @param width   the width of the input image
     * @param height  the height of the input image
     * @return returns the registered image
     */
    public int[][] getRegisteredImage(int[][] refImg, int[][] testImg, int width, int height) {
        // TODO
        return null;
    }


    public void run(ImageProcessor ip) {
        byte[] pixels = (byte[]) ip.getPixels();
        int width = ip.getWidth();
        int height = ip.getHeight();
        int[][] inDataArrInt = ImageJUtility.convertFrom1DByteArr(pixels, width, height);

        //initially transform input image to get a registration task
        double transX = 9.78;
        double transY = -1.99;
        double rot = 2.14;

        int[][] transformedImage = transformImg(inDataArrInt, width, height, transX, transY, rot);
        double initError = getImgDiffSSE(inDataArrInt, transformedImage, width, height);
        IJ.log("init error = " + initError);
        ImageJUtility.showNewImage(transformedImage, width, height, "transformed img");

        int[][] registeredImg = getRegisteredImage(inDataArrInt, transformedImage, width, height);
        ImageJUtility.showNewImage(registeredImg, width, height, "registered img");

    } //run

    void showAbout() {
        IJ.showMessage("About Template_...",
                "this is a PluginFilter template\n");
    } //showAbout

} //class Registration_
