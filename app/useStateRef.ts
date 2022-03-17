import { MutableRefObject, useEffect, useRef, useState } from 'react';

function useStateRef<T>(
  initialValue: T,
): [state: T, setState: React.Dispatch<React.SetStateAction<T>>, ref: MutableRefObject<T>] {
  const [state, setState] = useState<T>(initialValue);
  const ref = useRef<T>(initialValue);

  useEffect(() => {
    ref.current = state;
  }, [state]);

  return [state, setState, ref];
}

export { useStateRef };
