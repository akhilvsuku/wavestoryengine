# Use an official Ubuntu image as the base
FROM ubuntu:20.04


# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive


# Install build tools and g++ compiler
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /app

# Copy the entire project into the container
COPY . .

# Run the 'make' command to build the application
RUN make

# Expose the port your application uses (if needed, modify the port number)
EXPOSE 8080

# Set the default command to run your compiled executable
CMD ["./wstory"]
