package com.google.ortools;

import com.sun.jna.Platform;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.Collections;
import java.util.Objects;

/** Load native libraries needed for using ortools-java.*/
public class Loader {
  /** Try to locate the native libraries directory.*/
  private static URI getNativeResourceURI() throws IOException {
    ClassLoader loader = Loader.class.getClassLoader();
    String resource = Platform.RESOURCE_PREFIX + "/";
    URL resourceURL = loader.getResource(resource);
    Objects.requireNonNull(resourceURL,
        String.format("Resource %s was not found in ClassLoader %s",
          resource,
          loader));

    URI resourceURI;
    try {
      resourceURI = resourceURL.toURI();
    } catch (URISyntaxException e) {
      throw new IOException(e);
    }
    return resourceURI;
  }

  @FunctionalInterface
  private interface PathConsumer<T extends IOException> {
    void accept(Path path) throws T;
  }

  /** Extract native resources in a temp directory.
   * @param resourceURI Native resource location.
   * @return The directory path containing all extracted libraries.
   */
  private static Path unpackNativeResources(URI resourceURI) throws IOException {
    Path tempPath;
    tempPath = Files.createTempDirectory("ortools-java");
    tempPath.toFile().deleteOnExit();

    PathConsumer<?> visitor;
    visitor = (Path sourcePath) -> Files.walkFileTree(sourcePath, new SimpleFileVisitor<Path>() {
      @Override
      public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
        Path newPath = tempPath.resolve(sourcePath.getParent().relativize(file).toString());
        Files.copy(file, newPath);
        newPath.toFile().deleteOnExit();
        return FileVisitResult.CONTINUE;
      }

      @Override
      public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
        Path newPath = tempPath.resolve(sourcePath.getParent().relativize(dir).toString());
        Files.copy(dir, newPath);
        newPath.toFile().deleteOnExit();
        return FileVisitResult.CONTINUE;
      }
    });

    FileSystem fs = FileSystems.newFileSystem(resourceURI, Collections.emptyMap());
    Path p = fs.provider().getPath(resourceURI);
    visitor.accept(p);
    return tempPath;
  }

  /** Unpack and Load the native libraries needed for using ortools-java.*/
  public static void loadNativeLibraries() {
    try {
      URI resourceURI = getNativeResourceURI();
      Path tempPath = unpackNativeResources(resourceURI);
      // Load the native library
      System.load(
          tempPath.resolve(Platform.RESOURCE_PREFIX)
          .resolve(System.mapLibraryName("jniortools"))
          .toString());
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}

