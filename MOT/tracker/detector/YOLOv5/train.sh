#cd "/home/hipxk/project/202301 Spectrum MOT/deep_sort_pytorch-master/detector/YOLOv5"
#conda activate mot

python train.py --img 640 --batch 16 --epochs 3 --data data_custom/medium_0/data.yaml --weights yolov5s.pt

python train.py --img 1280 --batch 32 --epochs 50 --data data_custom/medium_0/data.yaml --weights yolov5s.pt --device 0,1
nohup python train.py --img 1280 --batch 32 --epochs 50 --data data_custom/medium_0/data.yaml --weights yolov5s.pt --device 0,1 > nohup.txt 2>&1 &
