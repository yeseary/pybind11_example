import example_multi
import time

num_threads = 4
num_iterations = 10000

start_time = time.time()
example_multi.parallel_compute(num_threads, num_iterations)
end_time = time.time()

print(f"Parallel computation with {num_threads} threads took {end_time - start_time} seconds")