/**
 * @Title: CGB Excercice 4
 * @author: S1910307074 Maximilian Mitter
 * @date: 26.05.2021
 * @timeSpent: 6h
 * @notes: display functions for most distance maps are commented out to no clutter the screen with windows
 */

import ij.*;
import ij.gui.GenericDialog;
import ij.plugin.filter.PlugInFilter;
import ij.process.ImageProcessor;

import java.security.InvalidParameterException;
import java.util.*;

public class S1910307074_Mitter_04_ implements PlugInFilter {

    private static final int BACKGROUND = 255;
    private static final int WALL = 0;
    private static final int START = 60;
    private static final int END = 120;
    private static final int OBSTACLE = 180;
    private static final int PATH_RED = 100;
    private static final int PATH_GREEN = 101;

    private static final int SCALE_MAX_VALUE = 200;
    private static final int SAFETY_DISTANCE = 4;
    private Method SELECTED_METHOD = Method.EUCLID;

    private int width = 0;
    private int height = 0;

    private static class Position {
        private int x;
        private int y;

        public Position(int x, int y) {
            this.x = x;
            this.y = y;
        }

        public int getX() {
            return x;
        }

        public int getY() {
            return y;
        }

        public void setX(int x) {
            this.x = x;
        }

        public void setY(int y) {
            this.y = y;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj instanceof Position) {
                Position other = (Position) obj;
                return (this.x == other.x && this.y == other.y);
            } else {
                return false;
            }
        }
    }

    private enum Method {
        EUCLID, CHECKERBOARD, MANHATTAN
    }

    @Override
    public int setup(String s, ImagePlus imagePlus) {
        return DOES_8G + DOES_STACKS + SUPPORTS_MASKING;
    }

    @Override
    public void run(ImageProcessor ip) {
        byte[] pixels = (byte[]) ip.getPixels();
        width = ip.getWidth();
        height = ip.getHeight();

        int[][] inDataArrInt = ImageJUtility.convertFrom1DByteArr(pixels, width, height);

        if (!validateMaze(inDataArrInt)) {
            GenericDialog errDialog = new GenericDialog("ERROR");
            errDialog.addMessage("Input Image is not valid");
            errDialog.showDialog();
            return;
        }

        /* Aufgabe 1 */

        double[][] distanceMap = calculateDistanceMap(inDataArrInt, "END");
        //showDistanceMap(distanceMap, "Distance Map unscaled");

        double[][] scaledDistanceMap = scaleDistanceMap(distanceMap);
        //showDistanceMap(scaledDistanceMap, "Distance Map scaled");

        /* Aufgabe 2 */

        showMethodDialog();

        System.out.println("Calculating shortest path without safety distance: ");
        var shortestPath = calculateShortestPath(getMinStartPosition(inDataArrInt, distanceMap), distanceMap);
        //addPathToImageAndDraw(shortestPath, inDataArrInt, PATH_RED);

        /* Aufgabe 3 */

        var obstacleDistanceMap = calculateDistanceMap(inDataArrInt, "OBSTACLE");

        var scaledObstacleDistanceMap = scaleDistanceMap(obstacleDistanceMap);
        showDistanceMap(scaledObstacleDistanceMap, "Obstacle Distance Map scaled");

        var distanceMapSafety = addObstacleSafetyToDistanceMap(inDataArrInt, obstacleDistanceMap);
        showDistanceMap(distanceMapSafety, "Distance Map unscaled with safety");

        System.out.println("Calculating shortest path with safety distance: ");
        var shortestPathWithSafety = calculateShortestPath(getMinStartPosition(inDataArrInt, distanceMapSafety), distanceMapSafety);
        //addPathToImageAndDraw(shortestPathWithSafety, inDataArrInt, PATH_GREEN);

        drawBothPaths(inDataArrInt, shortestPath, shortestPathWithSafety);
    }

    /**
     * Calculates the distance between to coordinates based on the Calculation method chosen by the user
     * @param p1 First Position
     * @param p2 Second Position
     * @return distance
     */
    private double distance(Position p1, Position p2) {
        double retVal = Double.POSITIVE_INFINITY;
        if (p1.equals(p2))
            retVal = 0;
        else if (p1.getX() == p2.getX() || p1.getY() == p2.getY() || SELECTED_METHOD == Method.CHECKERBOARD)
            retVal = 1;
        else if (SELECTED_METHOD == Method.EUCLID)
            retVal = Math.sqrt(2);

        return retVal;
    }

    /**
     * Calculates the distance map either for the End or for obstacles
     * @param imageData raw image data
     * @param target enum (END | OBSTACLE)
     * @return a matrix containing distance information
     */
    private double[][] calculateDistanceMap(int[][] imageData, String target) {
        double[][] distanceMap = new double[width][height];
        Queue<Position> stack = new ArrayDeque<>();

        int distanceFrom = -1;

        if (target.equals("END"))
            distanceFrom = END;
        else if (target.equals("OBSTACLE"))
            distanceFrom = OBSTACLE;
        else
            throw new InvalidParameterException("Target has to be END or OBSTACLE");

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (imageData[x][y] == distanceFrom) {
                    distanceMap[x][y] = 0;
                    stack.add(new Position(x, y));
                } else if (imageData[x][y] == WALL || imageData[x][y] == (distanceFrom == END ? OBSTACLE : 0)) {
                    // when calculating distance map for end, obstacles need to be invalidated
                    // when calculating distance map for obstacle, only walls need to be invalidated
                    distanceMap[x][y] = -1;
                } else {
                    distanceMap[x][y] = Double.POSITIVE_INFINITY;
                }
            }
        }

        int updateOperations = 0;

        while (!stack.isEmpty()) {
            var current = stack.poll();
            var neighbours = getNeighbours(current, distanceMap);

            for (var neighbour : neighbours) {
                double neighbourValue = distanceMap[neighbour.getX()][neighbour.getY()];

                if (distanceMap[current.getX()][current.getY()] > neighbourValue + distance(current, neighbour)) {
                    distanceMap[current.getX()][current.getY()] = neighbourValue + distance(current, neighbour);
                    updateOperations++;
                }
            }

            for (var neighbour : neighbours) {
                double currentValue = distanceMap[current.getX()][current.getY()];
                if (distanceMap[neighbour.getX()][neighbour.getY()] > currentValue + distance(current, neighbour)) {
                    distanceMap[neighbour.getX()][neighbour.getY()] = currentValue + distance(current, neighbour);
                    updateOperations++;
                    if (!stack.contains(neighbour))
                        stack.add(neighbour);
                }
            }
        }

        System.out.println("Total Update Operations: " + updateOperations);

        return distanceMap;
    }

    /**
     * gets all valid neighbours of a given position
     * @param pos the position to start from
     * @param distanceMap is used to check, if a neighbour is an invalid tile
     * @return a list of valid neighbours
     */
    private List<Position> getNeighbours(Position pos, double[][] distanceMap) {
        List<Position> neighbours = new ArrayList<>();
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                Position current = new Position(pos.getX() + x, pos.getY() + y);
                if (!isValidCoordinate(current)
                        || distanceMap[current.getX()][current.getY()] < 0
                        || (x == 0 && y == 0))

                    continue;
                else if (SELECTED_METHOD == Method.MANHATTAN && (x != 0 && y != 0))
                    continue;
                else
                    neighbours.add(current);
            }
        }
        return neighbours;
    }

    /**
     * checks if the given position is within the bounds of the given image
     * @param pos position
     * @return isValid
     */
    private boolean isValidCoordinate(Position pos) {
        return  pos.getX() >= 0 &&
                pos.getY() >= 0 &&
                pos.getX() < width &&
                pos.getY() < height;
    }

    /**
     * Transforms a distance map on a scale from 0 to SCALE_MAX_VALUE (default 200) for better display in a 8U Color sceme
     * @param distanceMap the distance map
     * @return a scaled distance map
     */
    private double[][] scaleDistanceMap(double[][] distanceMap) {
        double[][] newDistanceMap = new double[width][height];
        double maxValue = getMaxValue(distanceMap);

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (distanceMap[x][y] == WALL || distanceMap[x][y] == END)
                    newDistanceMap[x][y] = 255;
                else
                    newDistanceMap[x][y] = (distanceMap[x][y] / maxValue) * SCALE_MAX_VALUE;
            }
        }

        return newDistanceMap;
    }

    /**
     * gets the maximum value of a given distance map
     * @param distanceMap the distance map
     * @return the maximum value
     */
    private double getMaxValue(double[][] distanceMap) {
        double max = Double.NEGATIVE_INFINITY;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (distanceMap[x][y] > max)
                    max = distanceMap[x][y];
            }
        }
        return max;
    }

    /**
     * calculates the shortest path from a given position avoiding obstacles until an end tile is reached
     * @param startPosition the given starting position
     * @param distanceMap the distance map used to calculate distance
     * @return a List of Positions containing the shortest path
     */
    private Vector<Position> calculateShortestPath(Position startPosition, double[][] distanceMap) {
        Vector<Position> path = new Vector<>();
        var start = new Position(startPosition.getX(), startPosition.getY());

        path.add(start);

        double distanceToEnd = Double.POSITIVE_INFINITY;
        while (distanceToEnd > 0) {
            double minNeighbourValue = Double.POSITIVE_INFINITY;
            double minDistance = Double.POSITIVE_INFINITY;
            var neighbours = getNeighbours(start, distanceMap);
            Position bestNeighbour = new Position(0,0);

            for (var neighbour : neighbours) {
                double neighbourValue = distanceMap[neighbour.getX()][neighbour.getY()];
                double moveCost = distance(start, neighbour);

                if (neighbourValue + moveCost < minNeighbourValue + minDistance) {
                    minDistance = moveCost;
                    minNeighbourValue = neighbourValue;
                    bestNeighbour = neighbour;
                }
            }

            distanceToEnd = minNeighbourValue;
            if (!path.contains(bestNeighbour))
                path.add(bestNeighbour);
            start = bestNeighbour;
        }

        double totalCost = 0.0;
        for (var pos : path) {
            totalCost+= distanceMap[pos.getX()][pos.getY()];
        }
        System.out.println("Total COST of path: " + totalCost);

        return path;
    }

    /**
     * displays a given distance map in the valid 8U range of [0,255]
     * @param distanceMap the distance map to display
     * @param title a title for the window
     */
    private void showDistanceMap(double[][] distanceMap, String title) {
        double[][] drawMap = new double[width][height];
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (distanceMap[x][y] < 0)
                    drawMap[x][y] = 0;
                else if (distanceMap[x][y] > 255)
                    drawMap[x][y] = 255;
                else
                    drawMap[x][y] = distanceMap[x][y];
            }
        }
        ImageJUtility.showNewImage(drawMap, width, height, title);
    }

    /**
     * Draws both the shortest path with and without the safety distance from the obstacles.
     * Where both paths are on the same pixel, the color values are added together
     * @param inDataArrInt raw image data
     * @param shortestPath List of position for the shortest path without safety distance
     * @param shortestPathWithSafety List of position for the shortest path with safety distance
     */
    private void drawBothPaths(int[][] inDataArrInt, Vector<Position> shortestPath, Vector<Position> shortestPathWithSafety) {
        int[][] newImage = new int[width][height];
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                newImage[x][y] = inDataArrInt[x][y];
            }
        }

        for (var pos : shortestPath) {
            newImage[pos.getX()][pos.getY()] = PATH_RED;
        }

        for (var pos : shortestPathWithSafety) {
            if (newImage[pos.getX()][pos.getY()] == PATH_RED)
                newImage[pos.getX()][pos.getY()] = PATH_RED + PATH_GREEN;
            else
                newImage[pos.getX()][pos.getY()] = PATH_GREEN;
        }

        ImageJUtility.showNewImage(newImage, width, height, "Image with shortest Path");
        WindowManager.setTempCurrentImage(WindowManager.getImage("Image with shortest Path"));
        IJ.run("mazeLUT");
    }

    /**
     * combines raw image data and a obstacle distance map to calculate a distance map to the end with safety distance
     * @param inDataArrInt raw image data
     * @param obstacleDistanceMap distance map to the obstacles
     * @return distance map to the end with safety distance
     */
    private double[][] addObstacleSafetyToDistanceMap(int[][] inDataArrInt, double[][] obstacleDistanceMap) {
        int[][] outImg = new int[width][height];
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (obstacleDistanceMap[x][y] <= SAFETY_DISTANCE)
                    outImg[x][y] = WALL;
                else
                    outImg[x][y] = inDataArrInt[x][y];
            }
        }

        return calculateDistanceMap(outImg, "END");
    }

    /**
     * prints a dialog so the user can pick the preferred method of calculation
     */
    private void showMethodDialog() {
        String[] methods = new String[3];
        methods[0] = "Euclid";
        methods[1] = "Checkerboard";
        methods[2] = "Manhattan";
        GenericDialog dialog = new GenericDialog("Choose a Pathfinding Method: ");
        dialog.addChoice("Method", methods, methods[0]);
        dialog.showDialog();
        if (!dialog.wasCanceled()) {
            SELECTED_METHOD = Method.valueOf(dialog.getNextChoice().toUpperCase(Locale.ROOT));
        }
    }

    /**
     * adds a path to raw image data and displays it
     * @param shortestPath List of Positions indicating the shortest path
     * @param inDataArrInt raw image data
     * @param PATH_COLOR the color the path is drawn in
     */
    private void addPathToImageAndDraw(Vector<Position> shortestPath, int[][] inDataArrInt, int PATH_COLOR) {
        int[][] newImage = new int[width][height];
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                newImage[x][y] = inDataArrInt[x][y];
            }
        }

        for (var pos : shortestPath) {
            newImage[pos.getX()][pos.getY()] = PATH_COLOR;
        }

        ImageJUtility.showNewImage(newImage, width, height, "Image with shortest Path");
        WindowManager.setTempCurrentImage(WindowManager.getImage("Image with shortest Path"));
        IJ.run("mazeLUT");
    }

    /**
     * gets a starting position with minimum value on the distance map
     * @param inDataArrInt raw image data
     * @param distanceMap the distance map
     * @return minimum distance map value
     */
    private Position getMinStartPosition(int[][] inDataArrInt, double[][] distanceMap) {
        double minValue = Double.POSITIVE_INFINITY;
        Position start = null;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (inDataArrInt[x][y] == START) {
                    if (start == null) {
                        start = new Position(x, y);
                        minValue = distanceMap[x][y];
                    } else if (distanceMap[x][y] < minValue) {
                        start = new Position(x, y);
                        minValue = distanceMap[x][y];
                    }

                }
            }
        }
        return start;
    }

    /**
     * checks if the given image only contains valid color values
     * @param img raw image data
     * @return isValid
     */
    private boolean validateMaze(int[][] img) {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (img[x][y] != BACKGROUND &&
                        img[x][y] != WALL &&
                        img[x][y] != OBSTACLE &&
                        img[x][y] != START &&
                        img[x][y] != END)
                    return false;
            }
        }
        return true;
    }

}
