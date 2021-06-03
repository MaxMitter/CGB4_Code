public class ImageTransformationFilter {

    /**
     * apply scalar transformation
     *
     * @param inImg            The input image.
     * @param width            The width of the image image.
     * @param height           The height of the input image.
     * @param transferFunction The transferFunction where the index corresponds to a input color and the value to the
     *                         output color in which it should be transferred.
     * @return The resulting image.
     */
    public static int[][] getTransformedImage(int[][] inImg, int width, int height, int[] transferFunction) {
        int[][] returnImg = new int[width][height];

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                returnImg[i][j] = transferFunction[inImg[i][j]];
            }
        }

        return returnImg;
    }

    /**
     * get transfer function for contrast inversion
     *
     * @param maxVal the max value of the image space (note: this is 255 for most of the images)
     * @return The resulting transfer function.
     */
    public static int[] getInversionTF(int maxVal) {
        int[] transferFunction = new int[maxVal + 1];

        for (int i = 0; i < 255; i++) {
            transferFunction[i] = 255 - i;
        }

        return transferFunction;
    }


}
