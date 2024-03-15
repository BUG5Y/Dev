document.addEventListener('DOMContentLoaded', function() {
	const topPane = document.querySelector('.pane.top');
	const bottomPane = document.querySelector('.pane.bottom');
	const gutter = document.querySelector('.gutter');
	let isResizing = false;
	let startY, startHeight;
  
	gutter.addEventListener('mousedown', startDrag);
  
	function startDrag(e) {
	  isResizing = true;
	  startY = e.clientY;
	  startHeight = topPane.offsetHeight; // Use offsetHeight for initial height
  
	  document.addEventListener('mousemove', resize);
	  document.addEventListener('mouseup', stopDrag);
	}
  
	function resize(e) {
	  if (isResizing) {
		const deltaY = e.clientY - startY;
		const newHeight = startHeight + deltaY;
  
		topPane.style.height = newHeight + 'px';
		bottomPane.style.height = `calc(100% - ${newHeight}px)`;
	  }
	}
  
	function stopDrag() {
	  isResizing = false;
	  document.removeEventListener('mousemove', resize);
	  document.removeEventListener('mouseup', stopDrag);
	}
  });
  