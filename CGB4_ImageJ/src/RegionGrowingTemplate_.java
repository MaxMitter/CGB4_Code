import ij.IJ;
import ij.ImagePlus;
import ij.gui.PointRoi;
import ij.plugin.filter.PlugInFilter;
import ij.process.ImageProcessor;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;

/**
 * Applies a region growing onto a given image. This requires a ROI in form of points. (Lab 7)
 */
public class RegionGrowingTemplate_ implements PlugInFilter {

    public static final int FG_VAL = 255;
    public static final int BG_VAL = 0;
    public static final int UNPROCESSED_VAL = -1;

    private ImagePlus imp = null;

    public int setup(String arg, ImagePlus imp) {
        if (arg.equals("about")) {
            showAbout();
            return DONE;
        }

        this.imp = imp;

        return DOES_8G + DOES_STACKS + SUPPORTS_MASKING + ROI_REQUIRED;
    } //setup

    private List<Point> getSeedPoints() {
        List<Point> seedPositions = new ArrayList<>();
        PointRoi pr = (PointRoi) imp.getRoi();
        int[] xPositions = pr.getXCoordinates();
        int[] yPositions = pr.getYCoordinates();
        Rectangle boundingBox = pr.getBounds();

        //finally fill
        for (int i = 0; i < xPositions.length; i++) {
            seedPositions.add(new Point(xPositions[i] + boundingBox.x, yPositions[i] + boundingBox.y));
        }

        return seedPositions;
    }

    public void run(ImageProcessor ip) {
        byte[] pixels = (byte[]) ip.getPixels();
        int width = ip.getWidth();
        int height = ip.getHeight();
        int[][] inDataArrInt = ImageJUtility.convertFrom1DByteArr(pixels, width, height);

        // TODO

    } //run

    void showAbout() {
        IJ.showMessage("About Template_...",
                "this is a PluginFilter template\n");
    } //showAbout

} //class RegionGrow_