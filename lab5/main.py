import cv2
import argparse
import time
from concurrent.futures import ThreadPoolExecutor
from ultralytics import YOLO

class VideoCap:
    def __init__(self, video_path):
        self.video = cv2.VideoCapture(video_path)

    def __enter__(self):
        return self.video

    def __exit__(self, exc_type, exc_value, traceback):
        self.video.release()

class ThreadSafeYOLO:
    def __init__(self, model_path):
        self.model = YOLO(model_path)

    def predict(self, frame):
        result = self.model(frame, device='cpu')[0].plot()
        return result

class VideoProcessor:
    def __init__(self, video_path, output_path):
        self.video_path = video_path
        self.output_path = output_path

    def process_frame(self, frame, thread_safe_yolo):
        result = thread_safe_yolo.predict(frame)
        return result

    def process_video(self, num_threads=1):
        with VideoCap(self.video_path) as video:
            fps = video.get(cv2.CAP_PROP_FPS)
            num_frames = int(video.get(cv2.CAP_PROP_FRAME_COUNT))
            frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
            frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))

            out = cv2.VideoWriter(self.output_path, cv2.VideoWriter_fourcc(*'mp4v'), fps, (frame_width, frame_height))

            def process_frame_wrapper(frame, index, yolo_model):
                processed_frame = self.process_frame(frame, yolo_model)
                return index, processed_frame

            start_time = time.time()

            with ThreadPoolExecutor(max_workers=num_threads) as executor:
                yolo_models = [ThreadSafeYOLO('yolov8n-pose.pt') for _ in range(num_threads)]
                futures = []
                for i in range(num_frames):
                    ret, frame = video.read()
                    if not ret:
                        break
                    yolo_model = yolo_models[i % num_threads]
                    futures.append(executor.submit(process_frame_wrapper, frame, i, yolo_model))

                processed_frames = [None] * num_frames
                for future in futures:
                    index, processed_frame = future.result()
                    processed_frames[index] = processed_frame
                    cv2.imshow('Processed Frame', processed_frame)
                    cv2.waitKey(1)
                    out.write(processed_frame)

            end_time = time.time()

        return end_time - start_time

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process video with YOLOv8s-pose model.')
    parser.add_argument('--video_path', type=str, help='Path to input video')
    parser.add_argument('--output_path', type=str, help='Path to output video')
    parser.add_argument('--num_threads', type=int, default=1, help='Number of threads for parallel processing')
    args = parser.parse_args()

    processor = VideoProcessor(args.video_path, args.output_path)
    execution_time = processor.process_video(args.num_threads)
    print(f"Execution time: {execution_time} seconds")
