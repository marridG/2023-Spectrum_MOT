
# exec inference: Omni-MOT (part)
CUDA_VISIBLE_DEVICES=1,2 python deepsort.py ../data/test_videos/Middle_Camera_9__0-30s__watermark.mp4 --yolov5

# exec inference: Omni-MOT (full)
CUDA_VISIBLE_DEVICES=1,2 python deepsort.py ../data/test_videos/Middle_Camera_9.avi --yolov5

# calculate metrics
python eval_omni_mot.py