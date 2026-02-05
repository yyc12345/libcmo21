use bmap_sys;

#[test]
fn test_init_and_dispose() {
    assert!(unsafe { bmap_sys::BMInit() });
    assert!(unsafe { bmap_sys::BMDispose() });
}
