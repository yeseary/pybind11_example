import time
import example_gil
import threading

def run_task(no_gil:bool):

    n = 3
    def run_computation(i):
        print("开始长耗时任务计算...")
        example_gil.long_computation(no_gil)
        print("开始长耗时任务计算结束.")

    start_time = time.time()
    # 创建并启动多个线程
    threads = [threading.Thread(target=run_computation, args=(i,)) for i in range(n)]
    for t in threads:
        t.start()

    # 等待所有线程完成
    for t in threads:
        t.join()
    end_time = time.time()
    print(f"释放GIL锁:{no_gil},时间: {end_time - start_time} seconds")

run_task(no_gil=True)
run_task(no_gil=False)

num_threads = 4
num_iterations = 10000

start_time = time.time()
example_gil.parallel_compute(num_threads, num_iterations)
end_time = time.time()

print(f"Parallel computation with {num_threads} threads took {end_time - start_time} seconds")